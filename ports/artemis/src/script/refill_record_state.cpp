// refill_record_state.cpp -- the token reader the record steps refill.
//
// RefillRecordState runs one lexer step: it takes the lexer state and the
// semantic slot the caller wants the token payload in, reads the next token
// out of the input stream and answers with its token code. Whitespace and
// comments are skipped inside the step, numbers and names are collected
// through their own readers, and every lexical error is raised here as a
// syntax error carrying the source position. The caller (the record resume
// step, script/step_record_resume.cpp) keeps the token block beside the state
// and takes the answer back as one of its own outcomes.
//
// The step is suspendable -- every reader call below it can suspend the
// worker -- so the body carries the engine's asyncify state machine laid out
// exactly as the generated driver runs it. Phase rewinding pops the 44-byte
// frame the suspend path saved (both parameters and nine live words, in that
// order) and then pops the four-byte code naming the call that was in flight,
// because each of the 87 call sites keeps its own gate and a rewinding pass
// replays exactly the one the code names. Phase suspended pushes that code,
// then the frame, and answers zero. The two rewinding pops stay two blocks:
// the code word sits above the saved frame, so folding them would read the
// wrong words. The 288-byte scratch frame the body grows and hands back on
// every running exit is guest stack -- it drives unwind and rewind like any
// other frame.
//
// Judgment order: untouched. The goto graph is the generated driver's own
// flattening, kept as it stands (renamed labels only) so every compare runs
// in its original order; the dispatch at the top switches on the current
// character and each case body falls back into the read loop. Restructuring
// this into loops and switches is R2 work and needs the double anchor (frame
// hash and profile counts both green before and after).
//
// Node text: the seven diagnostics and formats live in the guest data segment
// at fixed addresses and the trap texts depend on those exact values, so they
// are named constants here, never retyped.
//
// src: artemis.wasm2c.c 806233-811597 (f1727), via code/f1727.cpp (R0
// transcription); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the readers and the error path this step drives.
// PROVISIONAL until their own rewrites land: the calls run through the
// engine_context.h call-throughs into the generated bodies.
uint32_t NextCharSlow(EngineContext* ctx, uint32_t reader);
uint32_t SkipSep(EngineContext* ctx, uint32_t lex_state);
void InclineNumber(EngineContext* ctx, uint32_t lex_state);
void ReadLongString(EngineContext* ctx, uint32_t lex_state, uint32_t seminfo, uint32_t sep);
void LexError(EngineContext* ctx, uint32_t lex_state, uint32_t message, uint32_t token);
uint32_t FormatPushString(EngineContext* ctx, uint32_t state, uint32_t format, uint32_t args);
void ThrowScriptError(EngineContext* ctx, uint32_t state, uint32_t code);
void GrowTableSlot(EngineContext* ctx, uint32_t state);
void ReadNumeral(EngineContext* ctx, uint32_t lex_state, uint32_t seminfo);
void FormatSourceId(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t length);
// Registered rewrites: script/intern_string.cpp, script/get_table_field_by_string.cpp.
uint32_t InternString(EngineContext* ctx, uint32_t state, uint32_t bytes, uint32_t length);
uint32_t GetTableFieldByString(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key);
// Registered call-throughs: util/engine_context.h.
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t state);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);

namespace {

// Token codes as the engine numbers them. Single-character tokens answer
// with their own character; these are the compound ones.
constexpr uint32_t kTokenConcat = 278u;
constexpr uint32_t kTokenDots = 279u;
constexpr uint32_t kTokenEq = 280u;
constexpr uint32_t kTokenGe = 281u;
constexpr uint32_t kTokenLe = 282u;
constexpr uint32_t kTokenNe = 283u;
constexpr uint32_t kTokenNumber = 284u;
constexpr uint32_t kTokenName = 285u;
constexpr uint32_t kTokenString = 286u;
constexpr uint32_t kTokenEos = 287u;

// Frame shapes: the 44-byte saved frame (eleven words), the four-byte resume
// code word stacked above it, the 288-byte scratch frame on the guest stack,
// and the source-id buffer the error paths format the chunk name into.
constexpr uint32_t kF1727SaveFrameBytes = 44u;
constexpr uint32_t kF1727ResumeWordBytes = 4u;
constexpr uint32_t kLocalFrameBytes = 288u;
constexpr uint32_t kSourceIdBytes = 80u;
// The element ceiling: a lexical element past this cannot be addressed.
constexpr uint32_t kMaxBufferSize = 0x7FFFFFFEu;

// The seven node texts in the guest data segment. The trap texts depend on
// these exact addresses; they are copied, never retyped into call sites.
constexpr uint32_t kMsgEof = 0x2AC22u;
constexpr uint32_t kMsgAddInfoFmt = 0x2ACBCu;
constexpr uint32_t kMsgNearFmt = 0x2ACC6u;
constexpr uint32_t kMsgElementTooLong = 0x2ACD3u;
constexpr uint32_t kMsgBadLongDelim = 0x2ACECu;
constexpr uint32_t kMsgUnfinishedString = 0x2AD73u;
constexpr uint32_t kMsgEscapeTooLarge = 0x2AD85u;

// Folded negative constants the generated body stages as u32 words.
constexpr uint32_t kU32MinusOne = 0xFFFFFFFFu;
constexpr uint32_t kU32MinusTwo = 0xFFFFFFFEu;
constexpr uint32_t kU32MinusSixtyFour = 0xFFFFFFC0u;

// The resume codes a suspension spills: zero is the main-loop tail call; each
// of the 87 call sites keeps its own code, and a rewinding pass replays
// exactly the call its code names.
constexpr uint32_t kResumeCall00 = 0u;
constexpr uint32_t kResumeNextChar01 = 1u;
constexpr uint32_t kResumeNextChar02 = 2u;
constexpr uint32_t kResumeSkipSep03 = 3u;
constexpr uint32_t kResumeNextChar04 = 4u;
constexpr uint32_t kResumeSkipSep05 = 5u;
constexpr uint32_t kResumeReadLongString06 = 6u;
constexpr uint32_t kResumeLexError07 = 7u;
constexpr uint32_t kResumeNextChar08 = 8u;
constexpr uint32_t kResumeNextChar09 = 9u;
constexpr uint32_t kResumeNextChar10 = 10u;
constexpr uint32_t kResumeNextChar11 = 11u;
constexpr uint32_t kResumeNextChar12 = 12u;
constexpr uint32_t kResumeNextChar13 = 13u;
constexpr uint32_t kResumeFormatMessage14 = 14u;
constexpr uint32_t kResumeThrowSyntax15 = 15u;
constexpr uint32_t kResumeReallocateBuffer16 = 16u;
constexpr uint32_t kResumeBlockTooBig17 = 17u;
constexpr uint32_t kResumeNextChar18 = 18u;
constexpr uint32_t kResumeFormatMessage19 = 19u;
constexpr uint32_t kResumeFormatMessage20 = 20u;
constexpr uint32_t kResumeThrowSyntax21 = 21u;
constexpr uint32_t kResumeLexError22 = 22u;
constexpr uint32_t kResumeFormatMessage23 = 23u;
constexpr uint32_t kResumeThrowSyntax24 = 24u;
constexpr uint32_t kResumeReallocateBuffer25 = 25u;
constexpr uint32_t kResumeBlockTooBig26 = 26u;
constexpr uint32_t kResumeNextChar27 = 27u;
constexpr uint32_t kResumeNextChar28 = 28u;
constexpr uint32_t kResumeFormatMessage29 = 29u;
constexpr uint32_t kResumeThrowSyntax30 = 30u;
constexpr uint32_t kResumeReallocateBuffer31 = 31u;
constexpr uint32_t kResumeBlockTooBig32 = 32u;
constexpr uint32_t kResumeInclineNumber33 = 33u;
constexpr uint32_t kResumeNextChar34 = 34u;
constexpr uint32_t kResumeLexError35 = 35u;
constexpr uint32_t kResumeFormatMessage36 = 36u;
constexpr uint32_t kResumeThrowSyntax37 = 37u;
constexpr uint32_t kResumeReallocateBuffer38 = 38u;
constexpr uint32_t kResumeBlockTooBig39 = 39u;
constexpr uint32_t kResumeFormatMessage40 = 40u;
constexpr uint32_t kResumeThrowSyntax41 = 41u;
constexpr uint32_t kResumeReallocateBuffer42 = 42u;
constexpr uint32_t kResumeBlockTooBig43 = 43u;
constexpr uint32_t kResumeNextChar44 = 44u;
constexpr uint32_t kResumeFormatMessage45 = 45u;
constexpr uint32_t kResumeThrowSyntax46 = 46u;
constexpr uint32_t kResumeReallocateBuffer47 = 47u;
constexpr uint32_t kResumeBlockTooBig48 = 48u;
constexpr uint32_t kResumeNextChar49 = 49u;
constexpr uint32_t kResumeFormatMessage50 = 50u;
constexpr uint32_t kResumeThrowSyntax51 = 51u;
constexpr uint32_t kResumeReallocateBuffer52 = 52u;
constexpr uint32_t kResumeBlockTooBig53 = 53u;
constexpr uint32_t kResumeNextChar54 = 54u;
constexpr uint32_t kResumeInternString55 = 55u;
constexpr uint32_t kResumeLookupKeyword56 = 56u;
constexpr uint32_t kResumeGrowTableSlot57 = 57u;
constexpr uint32_t kResumeReadLongString58 = 58u;
constexpr uint32_t kResumeNextChar59 = 59u;
constexpr uint32_t kResumeFormatMessage60 = 60u;
constexpr uint32_t kResumeThrowSyntax61 = 61u;
constexpr uint32_t kResumeReallocateBuffer62 = 62u;
constexpr uint32_t kResumeBlockTooBig63 = 63u;
constexpr uint32_t kResumeNextChar64 = 64u;
constexpr uint32_t kResumeInternString65 = 65u;
constexpr uint32_t kResumeLookupKeyword66 = 66u;
constexpr uint32_t kResumeGrowTableSlot67 = 67u;
constexpr uint32_t kResumeNextChar68 = 68u;
constexpr uint32_t kResumeFormatMessage69 = 69u;
constexpr uint32_t kResumeThrowSyntax70 = 70u;
constexpr uint32_t kResumeReallocateBuffer71 = 71u;
constexpr uint32_t kResumeBlockTooBig72 = 72u;
constexpr uint32_t kResumeNextChar73 = 73u;
constexpr uint32_t kResumeFormatMessage74 = 74u;
constexpr uint32_t kResumeThrowSyntax75 = 75u;
constexpr uint32_t kResumeReallocateBuffer76 = 76u;
constexpr uint32_t kResumeBlockTooBig77 = 77u;
constexpr uint32_t kResumeNextChar78 = 78u;
constexpr uint32_t kResumeFormatMessage79 = 79u;
constexpr uint32_t kResumeThrowSyntax80 = 80u;
constexpr uint32_t kResumeReallocateBuffer81 = 81u;
constexpr uint32_t kResumeBlockTooBig82 = 82u;
constexpr uint32_t kResumeNextChar83 = 83u;
constexpr uint32_t kResumeReadNumeral84 = 84u;
constexpr uint32_t kResumeNextChar85 = 85u;
constexpr uint32_t kResumeNextChar86 = 86u;

}  // namespace

uint32_t RefillRecordState(EngineContext* ctx, uint32_t lex_state, uint32_t seminfo) {
  EnterFrame();
  // The eleven words that ride the saved frame: both parameters and nine
  // live words -- +0 lex_state, +4 seminfo, +8 token, +12 selector,
  // +16 word_a, +20 word_b, +24 word_c, +28 word_d, +32 frame_base,
  // +36 word_e, +40 word_f. call_result stays off the frame (the replayed
  // call refills it) and resume_code pops from the four-byte frame of its
  // own.
  uint32_t token = 0u;
  uint32_t selector = 0u;
  uint32_t word_a = 0u;
  uint32_t call_result = 0u;
  uint32_t word_b = 0u;
  uint32_t word_c = 0u;
  uint32_t word_d = 0u;
  uint32_t resume_code = 0u;
  uint32_t frame_base = 0u;
  uint32_t word_e = 0u;
  uint32_t word_f = 0u;
  uint32_t i0 = 0u, i1 = 0u, i2 = 0u, i3 = 0u;
  uint64_t j0 = 0, j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1727SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    call_result = i0;
    i0 = LoadU32(ctx, i0);
    lex_state = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 8u);
    token = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 12u);
    selector = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 16u);
    word_a = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 20u);
    word_b = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 24u);
    word_c = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 28u);
    word_d = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 32u);
    frame_base = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 36u);
    word_e = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 40u);
    word_f = i0;
    i0 = call_result;
    i0 = LoadU32(ctx, i0 + 4u);
    seminfo = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1727ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    resume_code = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kLocalFrameBytes;
    i0 -= i1;
    frame_base = i0;
    GuestStackSp(ctx) = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 60u);
    token = i0;
    i1 = 0u;
    StoreU32(ctx, i0 + 4u, i1);
  }
// merge point dispatch_loop (wasm2c flattening boundary)
  dispatch_loop: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0);
      word_b = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
      i0 = kTokenEos;
      token = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      switch (i0) {
        case 0: goto running_exit;
        case 1: goto block_10;
        case 2: goto block_10;
        case 3: goto block_10;
        case 4: goto block_10;
        case 5: goto block_10;
        case 6: goto block_10;
        case 7: goto block_10;
        case 8: goto block_10;
        case 9: goto block_10;
        case 10: goto block_10;
        case 11: goto block_20;
        case 12: goto block_10;
        case 13: goto block_10;
        case 14: goto block_20;
        case 15: goto block_10;
        case 16: goto block_10;
        case 17: goto block_10;
        case 18: goto block_10;
        case 19: goto block_10;
        case 20: goto block_10;
        case 21: goto block_10;
        case 22: goto block_10;
        case 23: goto block_10;
        case 24: goto block_10;
        case 25: goto block_10;
        case 26: goto block_10;
        case 27: goto block_10;
        case 28: goto block_10;
        case 29: goto block_10;
        case 30: goto block_10;
        case 31: goto block_10;
        case 32: goto block_10;
        case 33: goto block_10;
        case 34: goto block_10;
        case 35: goto block_13;
        case 36: goto block_10;
        case 37: goto block_10;
        case 38: goto block_10;
        case 39: goto block_10;
        case 40: goto block_13;
        case 41: goto block_10;
        case 42: goto block_10;
        case 43: goto block_10;
        case 44: goto block_10;
        case 45: goto block_10;
        case 46: goto block_18;
        case 47: goto block_7;
        case 48: goto block_10;
        case 49: goto block_10;
        case 50: goto block_10;
        case 51: goto block_10;
        case 52: goto block_10;
        case 53: goto block_10;
        case 54: goto block_10;
        case 55: goto block_10;
        case 56: goto block_10;
        case 57: goto block_10;
        case 58: goto block_10;
        case 59: goto block_10;
        case 60: goto block_10;
        case 61: goto block_15;
        case 62: goto block_16;
        case 63: goto block_14;
        case 64: goto block_10;
        case 65: goto block_10;
        case 66: goto block_10;
        case 67: goto block_10;
        case 68: goto block_10;
        case 69: goto block_10;
        case 70: goto block_10;
        case 71: goto block_10;
        case 72: goto block_10;
        case 73: goto block_10;
        case 74: goto block_10;
        case 75: goto block_10;
        case 76: goto block_10;
        case 77: goto block_10;
        case 78: goto block_10;
        case 79: goto block_10;
        case 80: goto block_10;
        case 81: goto block_10;
        case 82: goto block_10;
        case 83: goto block_10;
        case 84: goto block_10;
        case 85: goto block_10;
        case 86: goto block_10;
        case 87: goto block_10;
        case 88: goto block_10;
        case 89: goto block_10;
        case 90: goto block_10;
        case 91: goto block_10;
        case 92: goto block_17;
        default: goto block_11;
      }
// merge point block_20 (wasm2c flattening boundary)
      block_20:;
    }
    i0 = resume_code;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      InclineNumber(ctx, i0);
      i0 = kResumeCall00;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto dispatch_loop;}
// merge point block_18 (wasm2c flattening boundary)
    block_18:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      token = i0;
      i0 = LoadU32(ctx, i0);
      word_b = i0;
      i1 = 1u;
      i0 -= i1;
      selector = i0;
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      if (i0) {
        i0 = token;
        i1 = token;
        i1 = LoadU32(ctx, i1 + 4u);
        token = i1;
        i2 = 1u;
        i1 += i2;
        word_a = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = token;
        i0 = LoadU8(ctx, i0);
        goto block_23;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar01;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar01;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = word_b;
    }
// merge point block_23 (wasm2c flattening boundary)
    block_23:;
    word_b = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = 45u;
      token = i0;
      i0 = word_b;
      i1 = 45u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      token = i0;
      i0 = LoadU32(ctx, i0);
      word_b = i0;
      i1 = 1u;
      i0 -= i1;
      selector = i0;
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      if (i0) {
        i0 = token;
        i1 = token;
        i1 = LoadU32(ctx, i1 + 4u);
        token = i1;
        i2 = 1u;
        i1 += i2;
        word_a = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = token;
        i0 = LoadU8(ctx, i0);
        goto block_28;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar02;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar02;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = word_b;
    }
// merge point block_28 (wasm2c flattening boundary)
    block_28:;
    word_b = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = word_b;
      i1 = 91u;
      i0 = i0 == i1;
      token = i0;
    }
    i0 = token;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = resume_code;
      i1 = kResumeSkipSep03;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = lex_state;
        i0 = SkipSep(ctx, i0);
        call_result = i0;
        i0 = kResumeSkipSep03;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        token = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = word_b;
      } else {
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 60u);
        i1 = 0u;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = token;
        i1 = kU32MinusOne;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        word_b = i0;
        if (i0) {goto block_12;}
        i0 = lex_state;
        i0 = LoadU32(ctx, i0);
      }
      word_b = i0;
    }
// merge point loop_37 (wasm2c flattening boundary)
    loop_37: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = word_b;
        i1 = 1u;
        i0 += i1;
        token = i0;
        i1 = 14u;
        i0 = i0 <= i1;
        word_b = i0;
        i1 = 0u;
        i2 = 1u;
        i3 = token;
        i2 <<= (i3 & 31);
        i3 = 18433u;
        i2 &= i3;
        i0 = i2 ? i0 : i1;
        token = i0;
        if (i0) {goto dispatch_loop;}
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 56u);
        token = i0;
        i0 = LoadU32(ctx, i0);
        word_b = i0;
        i1 = 1u;
        i0 -= i1;
        selector = i0;
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0, i1);
      }
      i0 = word_f;
      i1 = word_b;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      word_f = i0;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = token;
          i1 = LoadU32(ctx, i1 + 4u);
          token = i1;
          i2 = 1u;
          i1 += i2;
          selector = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = lex_state;
          i1 = token;
          i1 = LoadU8(ctx, i1);
          word_b = i1;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = word_f;
      i0 = !(i0);
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = resume_code;
        i1 = kResumeNextChar04;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeNextChar04;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i1 = word_b;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto loop_37;}
// merge point block_17 (wasm2c flattening boundary)
    block_17:;
    i0 = word_b;
    i1 = resume_code;
    i2 = kResumeSkipSep05;
    i1 = i1 == i2;
    i2 = 1u;
    i3 = ResumePhase(ctx);
    i1 = i3 ? i1 : i2;
    if (i1) {
      i1 = lex_state;
      i1 = SkipSep(ctx, i1);
      call_result = i1;
      i1 = kResumeSkipSep05;
      i2 = ResumePhase(ctx);
      i3 = 1u;
      i2 = i2 == i3;
      if (i2) {i0 = i1; goto spill_frame;}
      i1 = call_result;
      token = i1;
    }
    i1 = token;
    i2 = 0u;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    word_b = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = resume_code;
      i1 = kResumeReadLongString06;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = lex_state;
        i1 = seminfo;
        i2 = token;
        ReadLongString(ctx, i0, i1, i2);
        i0 = kResumeReadLongString06;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kTokenString;
        token = i0;
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      i1 = kU32MinusOne;
      i0 = i0 == i1;
      seminfo = i0;
      if (i0) {
        i0 = 91u;
        token = i0;
        goto running_exit;
      }
    }
    i0 = resume_code;
    i1 = kResumeLexError07;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
// pool text 175340: invalid long string delimiter
      i1 = kMsgBadLongDelim;
      i2 = kTokenString;
      LexError(ctx, i0, i1, i2);
      i0 = kResumeLexError07;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
// merge point block_16 (wasm2c flattening boundary)
    block_16:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      seminfo = i0;
      i0 = LoadU32(ctx, i0);
      token = i0;
      i1 = 1u;
      i0 -= i1;
      word_b = i0;
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      if (i0) {
        i0 = seminfo;
        token = i0;
        i0 = LoadU32(ctx, i0 + 4u);
        seminfo = i0;
        i1 = 1u;
        i0 += i1;
        selector = i0;
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = seminfo;
        i0 = LoadU8(ctx, i0);
        goto block_53;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar08;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar08;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = seminfo;
    }
// merge point block_53 (wasm2c flattening boundary)
    block_53:;
    seminfo = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = seminfo;
      StoreU32(ctx, i0, i1);
      i0 = 61u;
      token = i0;
      i0 = seminfo;
      i1 = 61u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      seminfo = i0;
      i0 = LoadU32(ctx, i0);
      token = i0;
      i1 = 1u;
      i0 -= i1;
      word_b = i0;
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      if (i0) {
        i0 = seminfo;
        i1 = seminfo;
        i1 = LoadU32(ctx, i1 + 4u);
        lex_state = i1;
        i2 = 1u;
        i1 += i2;
        token = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = lex_state;
        i0 = LoadU8(ctx, i0);
        goto block_58;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar09;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar09;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = lex_state;
    }
// merge point block_58 (wasm2c flattening boundary)
    block_58:;
    lex_state = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = lex_state;
      StoreU32(ctx, i0, i1);
      i0 = kTokenEq;
      token = i0;
      goto running_exit;
    }
// merge point block_15 (wasm2c flattening boundary)
    block_15:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      seminfo = i0;
      i0 = LoadU32(ctx, i0);
      token = i0;
      i1 = 1u;
      i0 -= i1;
      word_b = i0;
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      if (i0) {
        i0 = seminfo;
        token = i0;
        i0 = LoadU32(ctx, i0 + 4u);
        seminfo = i0;
        i1 = 1u;
        i0 += i1;
        selector = i0;
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = seminfo;
        i0 = LoadU8(ctx, i0);
        goto block_64;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar10;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar10;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = seminfo;
    }
// merge point block_64 (wasm2c flattening boundary)
    block_64:;
    seminfo = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = seminfo;
      StoreU32(ctx, i0, i1);
      i0 = 60u;
      token = i0;
      i0 = seminfo;
      i1 = 61u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      seminfo = i0;
      i0 = LoadU32(ctx, i0);
      token = i0;
      i1 = 1u;
      i0 -= i1;
      word_b = i0;
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      if (i0) {
        i0 = seminfo;
        i1 = seminfo;
        i1 = LoadU32(ctx, i1 + 4u);
        lex_state = i1;
        i2 = 1u;
        i1 += i2;
        token = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = lex_state;
        i0 = LoadU8(ctx, i0);
        goto block_69;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar11;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar11;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = lex_state;
    }
// merge point block_69 (wasm2c flattening boundary)
    block_69:;
    lex_state = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = lex_state;
      StoreU32(ctx, i0, i1);
      i0 = kTokenLe;
      token = i0;
      goto running_exit;
    }
// merge point block_14 (wasm2c flattening boundary)
    block_14:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      seminfo = i0;
      i0 = LoadU32(ctx, i0);
      token = i0;
      i1 = 1u;
      i0 -= i1;
      word_b = i0;
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      if (i0) {
        i0 = seminfo;
        token = i0;
        i0 = LoadU32(ctx, i0 + 4u);
        seminfo = i0;
        i1 = 1u;
        i0 += i1;
        selector = i0;
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = seminfo;
        i0 = LoadU8(ctx, i0);
        goto block_75;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar12;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar12;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = seminfo;
    }
// merge point block_75 (wasm2c flattening boundary)
    block_75:;
    seminfo = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = seminfo;
      StoreU32(ctx, i0, i1);
      i0 = 62u;
      token = i0;
      i0 = seminfo;
      i1 = 61u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      seminfo = i0;
      i0 = LoadU32(ctx, i0);
      token = i0;
      i1 = 1u;
      i0 -= i1;
      word_b = i0;
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      if (i0) {
        i0 = seminfo;
        i1 = seminfo;
        i1 = LoadU32(ctx, i1 + 4u);
        lex_state = i1;
        i2 = 1u;
        i1 += i2;
        token = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = lex_state;
        i0 = LoadU8(ctx, i0);
        goto block_80;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar13;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar13;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = lex_state;
    }
// merge point block_80 (wasm2c flattening boundary)
    block_80:;
    lex_state = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = lex_state;
      StoreU32(ctx, i0, i1);
      i0 = kTokenGe;
      token = i0;
      goto running_exit;
    }
// merge point block_13 (wasm2c flattening boundary)
    block_13:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 60u);
      token = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      word_d = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
      i1 = token;
      i1 = LoadU32(ctx, i1 + 8u);
      word_a = i1;
      i0 = i0 <= i1;
      word_c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_c;
      if (i0) {
        i0 = token;
        i0 = LoadU32(ctx, i0);
        word_a = i0;
        goto block_86;
      }
      i0 = word_a;
      i1 = kMaxBufferSize;
      i0 = i0 >= i1;
      selector = i0;
    }
    i0 = selector;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = frame_base;
        i1 = 208u;
        i0 += i1;
        i1 = lex_state;
        i1 = LoadU32(ctx, i1 + 64u);
        i2 = 16u;
        i1 += i2;
        i2 = kSourceIdBytes;
        FormatSourceId(ctx, i0, i1, i2);
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 52u);
        selector = i0;
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 4u);
        word_a = i0;
        i0 = frame_base;
// pool text 175315: lexical element too long
        i1 = kMsgElementTooLong;
        StoreU32(ctx, i0 + 152u, i1);
        i0 = frame_base;
        i1 = word_a;
        StoreU32(ctx, i0 + 148u, i1);
        i0 = frame_base;
        i1 = frame_base;
        i2 = 208u;
        i1 += i2;
        StoreU32(ctx, i0 + 144u, i1);
        i0 = frame_base;
        i1 = 144u;
        i0 += i1;
        word_a = i0;
      }
      i0 = resume_code;
      i1 = kResumeFormatMessage14;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
// pool text 175292: %s:%d: %s
        i1 = kMsgAddInfoFmt;
        i2 = word_a;
        i0 = FormatPushString(ctx, i0, i1, i2);
        call_result = i0;
        i0 = kResumeFormatMessage14;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        selector = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 52u);
        selector = i0;
      }
      i0 = resume_code;
      i1 = kResumeThrowSyntax15;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
        i1 = 3u;
        ThrowScriptError(ctx, i0, i1);
        i0 = kResumeThrowSyntax15;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = word_a;
      } else {
        i0 = token;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      word_a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_a;
      i1 = 1u;
      i0 <<= (i1 & 31);
      word_d = i0;
      i1 = kU32MinusTwo;
      i0 = i0 != i1;
      word_c = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      selector = i0;
    }
    i0 = word_c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = token;
        i0 = LoadU32(ctx, i0);
        word_c = i0;
      }
      i0 = resume_code;
      i1 = kResumeReallocateBuffer16;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
        i1 = word_c;
        i2 = word_a;
        i3 = word_d;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        call_result = i0;
        i0 = kResumeReallocateBuffer16;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        word_a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_97;}
    }
    i0 = resume_code;
    i1 = kResumeBlockTooBig17;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = selector;
      i0 = RaiseBlockTooBig(ctx, i0);
      call_result = i0;
      i0 = kResumeBlockTooBig17;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_a = i0;
    }
// merge point block_97 (wasm2c flattening boundary)
    block_97:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      i1 = word_d;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = token;
      i1 = word_a;
      StoreU32(ctx, i0, i1);
      i0 = token;
      i0 = LoadU32(ctx, i0 + 4u);
      word_d = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
    }
// merge point block_86 (wasm2c flattening boundary)
    block_86:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = word_a;
      i1 = word_d;
      i0 += i1;
      i1 = word_b;
      StoreU8(ctx, i0, i1);
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      token = i0;
      i0 = LoadU32(ctx, i0);
      selector = i0;
      i1 = 1u;
      i0 -= i1;
      word_a = i0;
      i0 = token;
      i1 = word_a;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      word_a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      if (i0) {
        i0 = token;
        selector = i0;
        i1 = token;
        i1 = LoadU32(ctx, i1 + 4u);
        token = i1;
        i2 = 1u;
        i1 += i2;
        word_c = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = token;
        i0 = LoadU8(ctx, i0);
        goto block_104;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar18;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar18;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = token;
    }
// merge point block_104 (wasm2c flattening boundary)
    block_104:;
    token = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_a;
      i1 = token;
      StoreU32(ctx, i0, i1);
      i0 = token;
      i1 = word_b;
      i0 = i0 != i1;
      selector = i0;
    }
    i0 = selector;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
// merge point loop_111 (wasm2c flattening boundary)
      loop_111: 
        i0 = selector;
        i1 = token;
        i2 = 1u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        selector = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = selector;
          switch (i0) {
            case 0: goto block_118;
            case 1: goto block_114;
            case 2: goto block_114;
            case 3: goto block_114;
            case 4: goto block_114;
            case 5: goto block_114;
            case 6: goto block_114;
            case 7: goto block_114;
            case 8: goto block_114;
            case 9: goto block_114;
            case 10: goto block_114;
            case 11: goto block_116;
            case 12: goto block_114;
            case 13: goto block_114;
            case 14: goto block_116;
            default: goto block_115;
          }
// merge point block_118 (wasm2c flattening boundary)
          block_118:;
          i0 = frame_base;
          i1 = 208u;
          i0 += i1;
          i1 = lex_state;
          i1 = LoadU32(ctx, i1 + 64u);
          i2 = 16u;
          i1 += i2;
          i2 = kSourceIdBytes;
          FormatSourceId(ctx, i0, i1, i2);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          token = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 4u);
          selector = i0;
          i0 = frame_base;
// pool text 175475: unfinished string
          i1 = kMsgUnfinishedString;
          StoreU32(ctx, i0 + 72u, i1);
          i0 = frame_base;
          i1 = selector;
          StoreU32(ctx, i0 + 68u, i1);
          i0 = frame_base;
          i1 = frame_base;
          i2 = 208u;
          i1 += i2;
          StoreU32(ctx, i0 + 64u, i1);
          i0 = frame_base;
          i1 = kU32MinusSixtyFour;
          i0 -= i1;
          selector = i0;
        }
        i0 = resume_code;
        i1 = kResumeFormatMessage19;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
// pool text 175292: %s:%d: %s
          i1 = kMsgAddInfoFmt;
          i2 = selector;
          i0 = FormatPushString(ctx, i0, i1, i2);
          call_result = i0;
          i0 = kResumeFormatMessage19;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          token = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          selector = i0;
          i0 = frame_base;
// pool text 175138: <eof>
          i1 = kMsgEof;
          StoreU32(ctx, i0 + 52u, i1);
          i0 = frame_base;
          i1 = token;
          StoreU32(ctx, i0 + 48u, i1);
          i0 = frame_base;
          i1 = 48u;
          i0 += i1;
          token = i0;
        }
        i0 = resume_code;
        i1 = kResumeFormatMessage20;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
// pool text 175302: %s near '%s'
          i1 = kMsgNearFmt;
          i2 = token;
          i0 = FormatPushString(ctx, i0, i1, i2);
          call_result = i0;
          i0 = kResumeFormatMessage20;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          token = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          token = i0;
        }
        i0 = resume_code;
        i1 = kResumeThrowSyntax21;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i1 = 3u;
          ThrowScriptError(ctx, i0, i1);
          i0 = kResumeThrowSyntax21;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
          goto block_112;
        }
// merge point block_116 (wasm2c flattening boundary)
        block_116:;
        i0 = resume_code;
        i1 = kResumeLexError22;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = lex_state;
// pool text 175475: unfinished string
          i1 = kMsgUnfinishedString;
          i2 = kTokenString;
          LexError(ctx, i0, i1, i2);
          i0 = kResumeLexError22;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
          goto block_112;
        }
// merge point block_115 (wasm2c flattening boundary)
        block_115:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = 92u;
          i0 = i0 == i1;
          selector = i0;
          if (i0) {goto block_113;}
        }
// merge point block_114 (wasm2c flattening boundary)
        block_114:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 60u);
          word_a = i0;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          selector = i0;
          i1 = word_a;
          i1 = LoadU32(ctx, i1 + 8u);
          word_c = i1;
          i0 = i0 <= i1;
          word_e = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_e;
          if (i0) {
            i0 = word_a;
            i0 = LoadU32(ctx, i0);
            word_c = i0;
            goto block_129;
          }
          i0 = word_c;
          i1 = kMaxBufferSize;
          i0 = i0 >= i1;
          selector = i0;
        }
        i0 = selector;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = frame_base;
            i1 = 208u;
            i0 += i1;
            i1 = lex_state;
            i1 = LoadU32(ctx, i1 + 64u);
            i2 = 16u;
            i1 += i2;
            i2 = kSourceIdBytes;
            FormatSourceId(ctx, i0, i1, i2);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 4u);
            word_d = i0;
            i0 = frame_base;
// pool text 175315: lexical element too long
            i1 = kMsgElementTooLong;
            StoreU32(ctx, i0 + 40u, i1);
            i0 = frame_base;
            i1 = word_d;
            StoreU32(ctx, i0 + 36u, i1);
            i0 = frame_base;
            i1 = frame_base;
            i2 = 208u;
            i1 += i2;
            StoreU32(ctx, i0 + 32u, i1);
            i0 = frame_base;
            i1 = 32u;
            i0 += i1;
            word_c = i0;
          }
          i0 = resume_code;
          i1 = kResumeFormatMessage23;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
// pool text 175292: %s:%d: %s
            i1 = kMsgAddInfoFmt;
            i2 = word_c;
            i0 = FormatPushString(ctx, i0, i1, i2);
            call_result = i0;
            i0 = kResumeFormatMessage23;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            selector = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
          }
          i0 = resume_code;
          i1 = kResumeThrowSyntax24;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeThrowSyntax24;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = word_c;
          } else {
            i0 = word_a;
            i0 = LoadU32(ctx, i0 + 8u);
          }
          word_c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_c;
          i1 = 1u;
          i0 <<= (i1 & 31);
          word_d = i0;
          i1 = kU32MinusTwo;
          i0 = i0 != i1;
          word_e = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          selector = i0;
        }
        i0 = word_e;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = word_a;
            i0 = LoadU32(ctx, i0);
            word_e = i0;
          }
          i0 = resume_code;
          i1 = kResumeReallocateBuffer25;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = word_e;
            i2 = word_c;
            i3 = word_d;
            i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
            call_result = i0;
            i0 = kResumeReallocateBuffer25;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            word_c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_140;}
        }
        i0 = resume_code;
        i1 = kResumeBlockTooBig26;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeBlockTooBig26;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_c = i0;
        }
// merge point block_140 (wasm2c flattening boundary)
        block_140:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = word_d;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = word_a;
          i1 = word_c;
          StoreU32(ctx, i0, i1);
          i0 = word_a;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          selector = i0;
        }
// merge point block_129 (wasm2c flattening boundary)
        block_129:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = selector;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = word_c;
          i1 = word_d;
          i0 += i1;
          i1 = token;
          StoreU8(ctx, i0, i1);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 56u);
          token = i0;
          i0 = LoadU32(ctx, i0);
          selector = i0;
          i1 = 1u;
          i0 -= i1;
          word_a = i0;
          i0 = token;
          i1 = word_a;
          StoreU32(ctx, i0, i1);
          i0 = lex_state;
          word_a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = selector;
          if (i0) {
            i0 = token;
            selector = i0;
            i1 = token;
            i1 = LoadU32(ctx, i1 + 4u);
            token = i1;
            i2 = 1u;
            i1 += i2;
            word_c = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = token;
            i0 = LoadU8(ctx, i0);
            goto block_147;
          }
        }
        i0 = resume_code;
        i1 = kResumeNextChar27;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeNextChar27;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
        } else {
          i0 = token;
        }
// merge point block_147 (wasm2c flattening boundary)
        block_147:;
        token = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = token;
          StoreU32(ctx, i0, i1);
          goto block_112;
        }
// merge point block_113 (wasm2c flattening boundary)
        block_113:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 56u);
          token = i0;
          i0 = LoadU32(ctx, i0);
          selector = i0;
          i1 = 1u;
          i0 -= i1;
          word_a = i0;
          i0 = token;
          i1 = word_a;
          StoreU32(ctx, i0, i1);
          i0 = lex_state;
          word_a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = selector;
          if (i0) {
            i0 = token;
            selector = i0;
            i1 = token;
            i1 = LoadU32(ctx, i1 + 4u);
            token = i1;
            i2 = 1u;
            i1 += i2;
            word_c = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = token;
            i0 = LoadU8(ctx, i0);
            goto block_153;
          }
        }
        i0 = resume_code;
        i1 = kResumeNextChar28;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeNextChar28;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
        } else {
          i0 = token;
        }
// merge point block_153 (wasm2c flattening boundary)
        block_153:;
        token = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = token;
          StoreU32(ctx, i0, i1);
          i0 = 7u;
          word_a = i0;
          i0 = token;
          i1 = 97u;
          i0 -= i1;
          selector = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = selector;
          switch (i0) {
            case 0: goto block_159;
            case 1: goto block_160;
            case 2: goto block_161;
            case 3: goto block_161;
            case 4: goto block_161;
            case 5: goto block_169;
            case 6: goto block_161;
            case 7: goto block_161;
            case 8: goto block_161;
            case 9: goto block_161;
            case 10: goto block_161;
            case 11: goto block_161;
            case 12: goto block_161;
            case 13: goto block_168;
            case 14: goto block_161;
            case 15: goto block_161;
            case 16: goto block_161;
            case 17: goto block_167;
            case 18: goto block_161;
            case 19: goto block_166;
            case 20: goto block_161;
            case 21: goto block_165;
            default: goto block_170;
          }
// merge point block_170 (wasm2c flattening boundary)
          block_170:;
          i0 = token;
          i1 = 1u;
          i0 += i1;
          selector = i0;
          switch (i0) {
            case 0: goto block_162;
            case 1: goto block_161;
            case 2: goto block_161;
            case 3: goto block_161;
            case 4: goto block_161;
            case 5: goto block_161;
            case 6: goto block_161;
            case 7: goto block_161;
            case 8: goto block_161;
            case 9: goto block_161;
            case 10: goto block_161;
            case 11: goto block_164;
            case 12: goto block_161;
            case 13: goto block_161;
            case 14: goto block_164;
            default: goto block_161;
          }
// merge point block_169 (wasm2c flattening boundary)
          block_169:;
          i0 = 12u;
          word_a = i0;
          goto block_159;
// merge point block_168 (wasm2c flattening boundary)
          block_168:;
          i0 = 10u;
          word_a = i0;
          goto block_159;
// merge point block_167 (wasm2c flattening boundary)
          block_167:;
          i0 = 13u;
          word_a = i0;
          goto block_159;
// merge point block_166 (wasm2c flattening boundary)
          block_166:;
          i0 = 9u;
          word_a = i0;
          goto block_159;
// merge point block_165 (wasm2c flattening boundary)
          block_165:;
          i0 = 11u;
          word_a = i0;
          goto block_159;
// merge point block_164 (wasm2c flattening boundary)
          block_164:;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 60u);
          token = i0;
          i0 = LoadU32(ctx, i0 + 4u);
          word_a = i0;
          i1 = 1u;
          i0 += i1;
          word_d = i0;
          i0 = word_d;
          i1 = token;
          i1 = LoadU32(ctx, i1 + 8u);
          selector = i1;
          i0 = i0 <= i1;
          word_c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_c;
          if (i0) {
            i0 = token;
            i0 = LoadU32(ctx, i0);
            selector = i0;
            goto block_171;
          }
          i0 = selector;
          i1 = kMaxBufferSize;
          i0 = i0 >= i1;
          word_a = i0;
        }
        i0 = word_a;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = frame_base;
            i1 = 208u;
            i0 += i1;
            i1 = lex_state;
            i1 = LoadU32(ctx, i1 + 64u);
            i2 = 16u;
            i1 += i2;
            i2 = kSourceIdBytes;
            FormatSourceId(ctx, i0, i1, i2);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 4u);
            word_a = i0;
            i0 = frame_base;
// pool text 175315: lexical element too long
            i1 = kMsgElementTooLong;
            StoreU32(ctx, i0 + 136u, i1);
            i0 = frame_base;
            i1 = word_a;
            StoreU32(ctx, i0 + 132u, i1);
            i0 = frame_base;
            i1 = frame_base;
            i2 = 208u;
            i1 += i2;
            StoreU32(ctx, i0 + 128u, i1);
            i0 = frame_base;
            i1 = 128u;
            i0 += i1;
            word_a = i0;
          }
          i0 = resume_code;
          i1 = kResumeFormatMessage29;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
// pool text 175292: %s:%d: %s
            i1 = kMsgAddInfoFmt;
            i2 = word_a;
            i0 = FormatPushString(ctx, i0, i1, i2);
            call_result = i0;
            i0 = kResumeFormatMessage29;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            selector = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
          }
          i0 = resume_code;
          i1 = kResumeThrowSyntax30;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeThrowSyntax30;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = selector;
          } else {
            i0 = token;
            i0 = LoadU32(ctx, i0 + 8u);
          }
          selector = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = selector;
          i1 = 1u;
          i0 <<= (i1 & 31);
          word_d = i0;
          i1 = kU32MinusTwo;
          i0 = i0 != i1;
          word_c = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          word_a = i0;
        }
        i0 = word_c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = token;
            i0 = LoadU32(ctx, i0);
            word_c = i0;
          }
          i0 = resume_code;
          i1 = kResumeReallocateBuffer31;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_a;
            i1 = word_c;
            i2 = selector;
            i3 = word_d;
            i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
            call_result = i0;
            i0 = kResumeReallocateBuffer31;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            selector = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_182;}
        }
        i0 = resume_code;
        i1 = kResumeBlockTooBig32;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = word_a;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeBlockTooBig32;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          selector = i0;
        }
// merge point block_182 (wasm2c flattening boundary)
        block_182:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = word_d;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = token;
          i1 = selector;
          StoreU32(ctx, i0, i1);
          i0 = token;
          i0 = LoadU32(ctx, i0 + 4u);
          word_a = i0;
          i1 = 1u;
          i0 += i1;
          word_d = i0;
        }
// merge point block_171 (wasm2c flattening boundary)
        block_171:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = word_d;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = selector;
          i1 = word_a;
          i0 += i1;
          token = i0;
          i1 = 10u;
          StoreU8(ctx, i0, i1);
        }
        i0 = resume_code;
        i1 = kResumeInclineNumber33;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = lex_state;
          InclineNumber(ctx, i0);
          i0 = kResumeInclineNumber33;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
// merge point block_162 (wasm2c flattening boundary)
        block_162:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
          goto block_112;
        }
// merge point block_161 (wasm2c flattening boundary)
        block_161:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = 0u;
          selector = i0;
          i0 = token;
          i1 = 48u;
          i0 -= i1;
          i1 = 10u;
          i0 = i0 >= i1;
          word_a = i0;
          if (i0) {goto block_158;}
          i0 = 0u;
          word_d = i0;
        }
// merge point loop_192 (wasm2c flattening boundary)
        loop_192: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 56u);
            word_a = i0;
            i0 = LoadU32(ctx, i0);
            word_c = i0;
            i1 = 1u;
            i0 -= i1;
            word_e = i0;
            i0 = word_a;
            i1 = word_e;
            StoreU32(ctx, i0, i1);
            i0 = lex_state;
            word_e = i0;
            i0 = selector;
            i1 = 10u;
            i0 *= i1;
            i1 = token;
            i0 += i1;
            token = i0;
            i1 = 48u;
            i0 -= i1;
            selector = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = word_c;
            if (i0) {
              i0 = word_a;
              i1 = word_a;
              i1 = LoadU32(ctx, i1 + 4u);
              token = i1;
              i2 = 1u;
              i1 += i2;
              word_c = i1;
              StoreU32(ctx, i0 + 4u, i1);
              i0 = token;
              i0 = LoadU8(ctx, i0);
              goto block_194;
            }
          }
          i0 = resume_code;
          i1 = kResumeNextChar34;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_a;
            i0 = NextCharSlow(ctx, i0);
            call_result = i0;
            i0 = kResumeNextChar34;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
          } else {
            i0 = token;
          }
// merge point block_194 (wasm2c flattening boundary)
          block_194:;
          token = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = word_e;
            i1 = token;
            StoreU32(ctx, i0, i1);
            i0 = word_d;
            i1 = 2u;
            i0 = i0 != i1;
            word_a = i0;
            if (i0) {
              i0 = word_d;
              i1 = 1u;
              i0 += i1;
              word_d = i0;
              i0 = token;
              i1 = 48u;
              i0 -= i1;
              i1 = 10u;
              i0 = i0 < i1;
              word_a = i0;
              if (i0) {goto loop_192;}
            }
          }
        i0 = token;
        i1 = selector;
        i2 = 256u;
        i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        token = i0;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = resume_code;
          i1 = kResumeLexError35;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = lex_state;
// pool text 175493: escape sequence too large
            i1 = kMsgEscapeTooLarge;
            i2 = kTokenString;
            LexError(ctx, i0, i1, i2);
            i0 = kResumeLexError35;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 60u);
          token = i0;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          word_c = i0;
          i0 = word_c;
          i1 = token;
          i1 = LoadU32(ctx, i1 + 8u);
          word_a = i1;
          i0 = i0 <= i1;
          word_e = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_e;
          if (i0) {
            i0 = token;
            i0 = LoadU32(ctx, i0);
            word_a = i0;
            goto block_203;
          }
          i0 = word_a;
          i1 = kMaxBufferSize;
          i0 = i0 >= i1;
          word_c = i0;
        }
        i0 = word_c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = frame_base;
            i1 = 208u;
            i0 += i1;
            i1 = lex_state;
            i1 = LoadU32(ctx, i1 + 64u);
            i2 = 16u;
            i1 += i2;
            i2 = kSourceIdBytes;
            FormatSourceId(ctx, i0, i1, i2);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            word_a = i0;
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 4u);
            word_d = i0;
            i0 = frame_base;
// pool text 175315: lexical element too long
            i1 = kMsgElementTooLong;
            StoreU32(ctx, i0 + 88u, i1);
            i0 = frame_base;
            i1 = word_d;
            StoreU32(ctx, i0 + 84u, i1);
            i0 = frame_base;
            i1 = frame_base;
            i2 = 208u;
            i1 += i2;
            StoreU32(ctx, i0 + 80u, i1);
            i0 = frame_base;
            i1 = 80u;
            i0 += i1;
            word_c = i0;
          }
          i0 = resume_code;
          i1 = kResumeFormatMessage36;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_a;
// pool text 175292: %s:%d: %s
            i1 = kMsgAddInfoFmt;
            i2 = word_c;
            i0 = FormatPushString(ctx, i0, i1, i2);
            call_result = i0;
            i0 = kResumeFormatMessage36;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            word_a = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            word_a = i0;
          }
          i0 = resume_code;
          i1 = kResumeThrowSyntax37;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_a;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeThrowSyntax37;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = word_a;
          } else {
            i0 = token;
            i0 = LoadU32(ctx, i0 + 8u);
          }
          word_a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = 1u;
          i0 <<= (i1 & 31);
          word_c = i0;
          i1 = kU32MinusTwo;
          i0 = i0 != i1;
          word_e = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          word_d = i0;
        }
        i0 = word_e;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = token;
            i0 = LoadU32(ctx, i0);
            word_e = i0;
          }
          i0 = resume_code;
          i1 = kResumeReallocateBuffer38;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_d;
            i1 = word_e;
            i2 = word_a;
            i3 = word_c;
            i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
            call_result = i0;
            i0 = kResumeReallocateBuffer38;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            word_a = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_213;}
        }
        i0 = resume_code;
        i1 = kResumeBlockTooBig39;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = word_d;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeBlockTooBig39;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_a = i0;
        }
// merge point block_213 (wasm2c flattening boundary)
        block_213:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = word_c;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = token;
          i1 = word_a;
          StoreU32(ctx, i0, i1);
          i0 = token;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          word_c = i0;
        }
// merge point block_203 (wasm2c flattening boundary)
        block_203:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = word_c;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = word_a;
          i1 = word_d;
          i0 += i1;
          i1 = selector;
          StoreU8(ctx, i0, i1);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
          goto block_112;
        }
// merge point block_160 (wasm2c flattening boundary)
        block_160:;
        i0 = word_a;
        i1 = 8u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        word_a = i0;
// merge point block_159 (wasm2c flattening boundary)
        block_159:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 60u);
          token = i0;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          selector = i0;
          i1 = token;
          i1 = LoadU32(ctx, i1 + 8u);
          word_c = i1;
          i0 = i0 <= i1;
          word_e = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_e;
          if (i0) {
            i0 = token;
            i0 = LoadU32(ctx, i0);
            word_c = i0;
            goto block_223;
          }
          i0 = word_c;
          i1 = kMaxBufferSize;
          i0 = i0 >= i1;
          selector = i0;
        }
        i0 = selector;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = frame_base;
            i1 = 208u;
            i0 += i1;
            i1 = lex_state;
            i1 = LoadU32(ctx, i1 + 64u);
            i2 = 16u;
            i1 += i2;
            i2 = kSourceIdBytes;
            FormatSourceId(ctx, i0, i1, i2);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 4u);
            word_d = i0;
            i0 = frame_base;
// pool text 175315: lexical element too long
            i1 = kMsgElementTooLong;
            StoreU32(ctx, i0 + 120u, i1);
            i0 = frame_base;
            i1 = word_d;
            StoreU32(ctx, i0 + 116u, i1);
            i0 = frame_base;
            i1 = frame_base;
            i2 = 208u;
            i1 += i2;
            StoreU32(ctx, i0 + 112u, i1);
            i0 = frame_base;
            i1 = 112u;
            i0 += i1;
            word_c = i0;
          }
          i0 = resume_code;
          i1 = kResumeFormatMessage40;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
// pool text 175292: %s:%d: %s
            i1 = kMsgAddInfoFmt;
            i2 = word_c;
            i0 = FormatPushString(ctx, i0, i1, i2);
            call_result = i0;
            i0 = kResumeFormatMessage40;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            selector = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
          }
          i0 = resume_code;
          i1 = kResumeThrowSyntax41;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeThrowSyntax41;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = word_c;
          } else {
            i0 = token;
            i0 = LoadU32(ctx, i0 + 8u);
          }
          word_c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_c;
          i1 = 1u;
          i0 <<= (i1 & 31);
          word_d = i0;
          i1 = kU32MinusTwo;
          i0 = i0 != i1;
          word_e = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          selector = i0;
        }
        i0 = word_e;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = token;
            i0 = LoadU32(ctx, i0);
            word_e = i0;
          }
          i0 = resume_code;
          i1 = kResumeReallocateBuffer42;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = word_e;
            i2 = word_c;
            i3 = word_d;
            i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
            call_result = i0;
            i0 = kResumeReallocateBuffer42;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            word_c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_234;}
        }
        i0 = resume_code;
        i1 = kResumeBlockTooBig43;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeBlockTooBig43;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_c = i0;
        }
// merge point block_234 (wasm2c flattening boundary)
        block_234:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = word_d;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = token;
          i1 = word_c;
          StoreU32(ctx, i0, i1);
          i0 = token;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          selector = i0;
        }
// merge point block_223 (wasm2c flattening boundary)
        block_223:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = selector;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = word_c;
          i1 = word_d;
          i0 += i1;
          i1 = word_a;
          StoreU8(ctx, i0, i1);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 56u);
          token = i0;
          i0 = LoadU32(ctx, i0);
          selector = i0;
          i1 = 1u;
          i0 -= i1;
          word_a = i0;
          i0 = token;
          i1 = word_a;
          StoreU32(ctx, i0, i1);
          i0 = selector;
          if (i0) {
            i0 = token;
            selector = i0;
            i1 = token;
            i1 = LoadU32(ctx, i1 + 4u);
            token = i1;
            i2 = 1u;
            i1 += i2;
            word_a = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = lex_state;
            i1 = token;
            i1 = LoadU8(ctx, i1);
            StoreU32(ctx, i0, i1);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0);
            token = i0;
            goto block_112;
          }
        }
        i0 = resume_code;
        i1 = kResumeNextChar44;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeNextChar44;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          token = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i1 = token;
          StoreU32(ctx, i0, i1);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
          goto block_112;
        }
// merge point block_158 (wasm2c flattening boundary)
        block_158:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 60u);
          word_a = i0;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          selector = i0;
          i1 = word_a;
          i1 = LoadU32(ctx, i1 + 8u);
          word_c = i1;
          i0 = i0 <= i1;
          word_e = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_e;
          if (i0) {
            i0 = word_a;
            i0 = LoadU32(ctx, i0);
            word_c = i0;
            goto block_245;
          }
          i0 = word_c;
          i1 = kMaxBufferSize;
          i0 = i0 >= i1;
          selector = i0;
        }
        i0 = selector;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = frame_base;
            i1 = 208u;
            i0 += i1;
            i1 = lex_state;
            i1 = LoadU32(ctx, i1 + 64u);
            i2 = 16u;
            i1 += i2;
            i2 = kSourceIdBytes;
            FormatSourceId(ctx, i0, i1, i2);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 4u);
            word_d = i0;
            i0 = frame_base;
// pool text 175315: lexical element too long
            i1 = kMsgElementTooLong;
            StoreU32(ctx, i0 + 104u, i1);
            i0 = frame_base;
            i1 = word_d;
            StoreU32(ctx, i0 + 100u, i1);
            i0 = frame_base;
            i1 = frame_base;
            i2 = 208u;
            i1 += i2;
            StoreU32(ctx, i0 + 96u, i1);
            i0 = frame_base;
            i1 = 96u;
            i0 += i1;
            word_c = i0;
          }
          i0 = resume_code;
          i1 = kResumeFormatMessage45;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
// pool text 175292: %s:%d: %s
            i1 = kMsgAddInfoFmt;
            i2 = word_c;
            i0 = FormatPushString(ctx, i0, i1, i2);
            call_result = i0;
            i0 = kResumeFormatMessage45;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            selector = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = lex_state;
            i0 = LoadU32(ctx, i0 + 52u);
            selector = i0;
          }
          i0 = resume_code;
          i1 = kResumeThrowSyntax46;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeThrowSyntax46;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = word_c;
          } else {
            i0 = word_a;
            i0 = LoadU32(ctx, i0 + 8u);
          }
          word_c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_c;
          i1 = 1u;
          i0 <<= (i1 & 31);
          word_d = i0;
          i1 = kU32MinusTwo;
          i0 = i0 != i1;
          word_e = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          selector = i0;
        }
        i0 = word_e;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = word_a;
            i0 = LoadU32(ctx, i0);
            word_e = i0;
          }
          i0 = resume_code;
          i1 = kResumeReallocateBuffer47;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = word_e;
            i2 = word_c;
            i3 = word_d;
            i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
            call_result = i0;
            i0 = kResumeReallocateBuffer47;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
            word_c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_256;}
        }
        i0 = resume_code;
        i1 = kResumeBlockTooBig48;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeBlockTooBig48;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_c = i0;
        }
// merge point block_256 (wasm2c flattening boundary)
        block_256:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = word_d;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = word_a;
          i1 = word_c;
          StoreU32(ctx, i0, i1);
          i0 = word_a;
          i0 = LoadU32(ctx, i0 + 4u);
          word_d = i0;
          i1 = 1u;
          i0 += i1;
          selector = i0;
        }
// merge point block_245 (wasm2c flattening boundary)
        block_245:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = word_a;
          i1 = selector;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = word_c;
          i1 = word_d;
          i0 += i1;
          i1 = token;
          StoreU8(ctx, i0, i1);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 56u);
          token = i0;
          i0 = LoadU32(ctx, i0);
          selector = i0;
          i1 = 1u;
          i0 -= i1;
          word_a = i0;
          i0 = token;
          i1 = word_a;
          StoreU32(ctx, i0, i1);
          i0 = selector;
          if (i0) {
            i0 = token;
            selector = i0;
            i1 = token;
            i1 = LoadU32(ctx, i1 + 4u);
            token = i1;
            i2 = 1u;
            i1 += i2;
            word_a = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = lex_state;
            i1 = token;
            i1 = LoadU8(ctx, i1);
            StoreU32(ctx, i0, i1);
            i0 = lex_state;
            i0 = LoadU32(ctx, i0);
            token = i0;
            goto block_112;
          }
        }
        i0 = resume_code;
        i1 = kResumeNextChar49;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeNextChar49;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          token = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i1 = token;
          StoreU32(ctx, i0, i1);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
        }
// merge point block_112 (wasm2c flattening boundary)
        block_112:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i1 = word_b;
          i0 = i0 != i1;
          selector = i0;
          if (i0) {goto loop_111;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 60u);
      token = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      word_d = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
      i1 = token;
      i1 = LoadU32(ctx, i1 + 8u);
      word_a = i1;
      i0 = i0 <= i1;
      word_c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_c;
      if (i0) {
        i0 = token;
        i0 = LoadU32(ctx, i0);
        word_a = i0;
        goto block_268;
      }
      i0 = word_a;
      i1 = kMaxBufferSize;
      i0 = i0 >= i1;
      selector = i0;
    }
    i0 = selector;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = frame_base;
        i1 = 208u;
        i0 += i1;
        i1 = lex_state;
        i1 = LoadU32(ctx, i1 + 64u);
        i2 = 16u;
        i1 += i2;
        i2 = kSourceIdBytes;
        FormatSourceId(ctx, i0, i1, i2);
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 52u);
        selector = i0;
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 4u);
        word_a = i0;
        i0 = frame_base;
// pool text 175315: lexical element too long
        i1 = kMsgElementTooLong;
        StoreU32(ctx, i0 + 24u, i1);
        i0 = frame_base;
        i1 = word_a;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = frame_base;
        i1 = frame_base;
        i2 = 208u;
        i1 += i2;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = frame_base;
        i1 = 16u;
        i0 += i1;
        word_a = i0;
      }
      i0 = resume_code;
      i1 = kResumeFormatMessage50;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
// pool text 175292: %s:%d: %s
        i1 = kMsgAddInfoFmt;
        i2 = word_a;
        i0 = FormatPushString(ctx, i0, i1, i2);
        call_result = i0;
        i0 = kResumeFormatMessage50;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        selector = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 52u);
        selector = i0;
      }
      i0 = resume_code;
      i1 = kResumeThrowSyntax51;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
        i1 = 3u;
        ThrowScriptError(ctx, i0, i1);
        i0 = kResumeThrowSyntax51;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = word_a;
      } else {
        i0 = token;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      word_a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_a;
      i1 = 1u;
      i0 <<= (i1 & 31);
      word_d = i0;
      i1 = kU32MinusTwo;
      i0 = i0 != i1;
      word_c = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      selector = i0;
    }
    i0 = word_c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = token;
        i0 = LoadU32(ctx, i0);
        word_c = i0;
      }
      i0 = resume_code;
      i1 = kResumeReallocateBuffer52;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
        i1 = word_c;
        i2 = word_a;
        i3 = word_d;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        call_result = i0;
        i0 = kResumeReallocateBuffer52;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        word_a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_279;}
    }
    i0 = resume_code;
    i1 = kResumeBlockTooBig53;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = selector;
      i0 = RaiseBlockTooBig(ctx, i0);
      call_result = i0;
      i0 = kResumeBlockTooBig53;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_a = i0;
    }
// merge point block_279 (wasm2c flattening boundary)
    block_279:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      i1 = word_d;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = token;
      i1 = word_a;
      StoreU32(ctx, i0, i1);
      i0 = token;
      i0 = LoadU32(ctx, i0 + 4u);
      word_d = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
    }
// merge point block_268 (wasm2c flattening boundary)
    block_268:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = word_a;
      i1 = word_d;
      i0 += i1;
      i1 = word_b;
      StoreU8(ctx, i0, i1);
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 56u);
      token = i0;
      i0 = LoadU32(ctx, i0);
      word_b = i0;
      i1 = 1u;
      i0 -= i1;
      selector = i0;
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      if (i0) {
        i0 = token;
        word_b = i0;
        i1 = token;
        i1 = LoadU32(ctx, i1 + 4u);
        token = i1;
        i2 = 1u;
        i1 += i2;
        word_a = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = token;
        i0 = LoadU8(ctx, i0);
        goto block_286;
      }
    }
    i0 = resume_code;
    i1 = kResumeNextChar54;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeNextChar54;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = token;
    }
// merge point block_286 (wasm2c flattening boundary)
    block_286:;
    token = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      i1 = token;
      StoreU32(ctx, i0, i1);
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 60u);
      word_b = i0;
      i0 = LoadU32(ctx, i0);
      i1 = 1u;
      i0 += i1;
      selector = i0;
      i0 = word_b;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = 2u;
      i0 -= i1;
      word_b = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
    }
    i0 = resume_code;
    i1 = kResumeInternString55;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = selector;
      i2 = word_b;
      i0 = InternString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeInternString55;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 48u);
      i0 = LoadU32(ctx, i0 + 4u);
      lex_state = i0;
    }
    i0 = resume_code;
    i1 = kResumeLookupKeyword56;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = lex_state;
      i2 = word_b;
      i0 = GetTableFieldByString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeLookupKeyword56;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      lex_state = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 8u);
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      if (i0) {goto block_295;}
      i0 = lex_state;
      i1 = 1u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = lex_state;
      i1 = 1u;
      StoreU32(ctx, i0, i1);
      i0 = token;
      i0 = LoadU32(ctx, i0 + 16u);
      lex_state = i0;
      i0 = LoadU32(ctx, i0 + 68u);
      selector = i0;
      i0 = selector;
      i1 = lex_state;
      i1 = LoadU32(ctx, i1 + 64u);
      i0 = i0 < i1;
      lex_state = i0;
      if (i0) {goto block_295;}
    }
    i0 = resume_code;
    i1 = kResumeGrowTableSlot57;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      GrowTableSlot(ctx, i0);
      i0 = kResumeGrowTableSlot57;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
// merge point block_295 (wasm2c flattening boundary)
    block_295:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = kTokenString;
      token = i0;
      goto running_exit;
    }
// merge point block_12 (wasm2c flattening boundary)
    block_12:;
    i0 = resume_code;
    i1 = kResumeReadLongString58;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = 0u;
      i2 = token;
      ReadLongString(ctx, i0, i1, i2);
      i0 = kResumeReadLongString58;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 60u);
      token = i0;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      goto dispatch_loop;
    }
// merge point block_11 (wasm2c flattening boundary)
    block_11:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = 126u;
      i0 = i0 == i1;
      token = i0;
      if (i0) {goto block_5;}
    }
// merge point block_10 (wasm2c flattening boundary)
    block_10:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i1 = 9u;
      i0 -= i1;
      i1 = 5u;
      i0 = i0 < i1;
      selector = i0;
      i1 = word_b;
      i2 = 32u;
      i1 = i1 == i2;
      i0 |= i1;
      token = i0;
    }
    i0 = token;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 56u);
        token = i0;
        i0 = LoadU32(ctx, i0);
        word_b = i0;
        i1 = 1u;
        i0 -= i1;
        selector = i0;
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0, i1);
        i0 = word_b;
        if (i0) {
          i0 = token;
          word_b = i0;
          i1 = token;
          i1 = LoadU32(ctx, i1 + 4u);
          token = i1;
          i2 = 1u;
          i1 += i2;
          selector = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = lex_state;
          i1 = token;
          i1 = LoadU8(ctx, i1);
          token = i1;
          StoreU32(ctx, i0, i1);
          goto dispatch_loop;
        }
      }
      i0 = resume_code;
      i1 = kResumeNextChar59;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = token;
        i0 = NextCharSlow(ctx, i0);
        call_result = i0;
        i0 = kResumeNextChar59;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        token = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = lex_state;
        i1 = token;
        StoreU32(ctx, i0, i1);
        goto dispatch_loop;
      }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    i1 = 48u;
    i0 -= i1;
    i1 = 9u;
    i0 = i0 <= i1;
    token = i0;
    if (i0) {goto block_6;}
    i0 = 0u;
    i1 = word_b;
    i2 = 95u;
    i1 = i1 != i2;
    i2 = word_b;
    i3 = 32u;
    i2 |= i3;
    i3 = 97u;
    i2 -= i3;
    i3 = 26u;
    i2 = i2 < i3;
    selector = i2;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    token = i0;
  }
  i0 = token;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
// merge point loop_312 (wasm2c flattening boundary)
    loop_312: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 60u);
        token = i0;
        i0 = LoadU32(ctx, i0 + 4u);
        word_a = i0;
        i1 = 1u;
        i0 += i1;
        word_d = i0;
        i0 = word_d;
        i1 = token;
        i1 = LoadU32(ctx, i1 + 8u);
        selector = i1;
        i0 = i0 <= i1;
        word_c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = word_c;
        if (i0) {
          i0 = token;
          i0 = LoadU32(ctx, i0);
          selector = i0;
          goto block_314;
        }
        i0 = selector;
        i1 = kMaxBufferSize;
        i0 = i0 >= i1;
        word_a = i0;
      }
      i0 = word_a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = frame_base;
          i1 = 208u;
          i0 += i1;
          i1 = lex_state;
          i1 = LoadU32(ctx, i1 + 64u);
          i2 = 16u;
          i1 += i2;
          i2 = kSourceIdBytes;
          FormatSourceId(ctx, i0, i1, i2);
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          selector = i0;
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 4u);
          word_a = i0;
          i0 = frame_base;
// pool text 175315: lexical element too long
          i1 = kMsgElementTooLong;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = frame_base;
          i1 = word_a;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = frame_base;
          i1 = frame_base;
          i2 = 208u;
          i1 += i2;
          word_a = i1;
          StoreU32(ctx, i0, i1);
        }
        i0 = resume_code;
        i1 = kResumeFormatMessage60;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
// pool text 175292: %s:%d: %s
          i1 = kMsgAddInfoFmt;
          i2 = frame_base;
          i0 = FormatPushString(ctx, i0, i1, i2);
          call_result = i0;
          i0 = kResumeFormatMessage60;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          selector = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0 + 52u);
          selector = i0;
        }
        i0 = resume_code;
        i1 = kResumeThrowSyntax61;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i1 = 3u;
          ThrowScriptError(ctx, i0, i1);
          i0 = kResumeThrowSyntax61;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = selector;
        } else {
          i0 = token;
          i0 = LoadU32(ctx, i0 + 8u);
        }
        selector = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = selector;
        i1 = 1u;
        i0 <<= (i1 & 31);
        word_d = i0;
        i1 = kU32MinusTwo;
        i0 = i0 != i1;
        word_c = i0;
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 52u);
        word_a = i0;
      }
      i0 = word_c;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = token;
          i0 = LoadU32(ctx, i0);
          word_c = i0;
        }
        i0 = resume_code;
        i1 = kResumeReallocateBuffer62;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = word_a;
          i1 = word_c;
          i2 = selector;
          i3 = word_d;
          i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
          call_result = i0;
          i0 = kResumeReallocateBuffer62;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          selector = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_325;}
      }
      i0 = resume_code;
      i1 = kResumeBlockTooBig63;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = word_a;
        i0 = RaiseBlockTooBig(ctx, i0);
        call_result = i0;
        i0 = kResumeBlockTooBig63;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        selector = i0;
      }
// merge point block_325 (wasm2c flattening boundary)
      block_325:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = token;
        i1 = word_d;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0, i1);
        i0 = token;
        i0 = LoadU32(ctx, i0 + 4u);
        word_a = i0;
        i1 = 1u;
        i0 += i1;
        word_d = i0;
      }
// merge point block_314 (wasm2c flattening boundary)
      block_314:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = token;
        i1 = word_d;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = selector;
        i1 = word_a;
        i0 += i1;
        i1 = word_b;
        StoreU8(ctx, i0, i1);
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 56u);
        token = i0;
        i0 = LoadU32(ctx, i0);
        word_b = i0;
        i1 = 1u;
        i0 -= i1;
        selector = i0;
        i0 = token;
        i1 = selector;
        StoreU32(ctx, i0, i1);
        i0 = lex_state;
        selector = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = word_b;
        if (i0) {
          i0 = token;
          i1 = token;
          i1 = LoadU32(ctx, i1 + 4u);
          token = i1;
          i2 = 1u;
          i1 += i2;
          word_a = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = token;
          i0 = LoadU8(ctx, i0);
          goto block_332;
        }
      }
      i0 = resume_code;
      i1 = kResumeNextChar64;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = token;
        i0 = NextCharSlow(ctx, i0);
        call_result = i0;
        i0 = kResumeNextChar64;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
      } else {
        i0 = word_b;
      }
// merge point block_332 (wasm2c flattening boundary)
      block_332:;
      word_b = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = selector;
        i1 = word_b;
        StoreU32(ctx, i0, i1);
        i0 = 1u;
        i1 = word_b;
        i2 = 48u;
        i1 -= i2;
        i2 = 10u;
        i1 = i1 < i2;
        i2 = word_b;
        i3 = 32u;
        i2 |= i3;
        i3 = 97u;
        i2 -= i3;
        i3 = 26u;
        i2 = i2 < i3;
        i0 = i2 ? i0 : i1;
        token = i0;
        i0 = word_b;
        i1 = 95u;
        i0 = i0 == i1;
        selector = i0;
        if (i0) {goto loop_312;}
        i0 = token;
        if (i0) {goto loop_312;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 60u);
      word_b = i0;
      i0 = LoadU32(ctx, i0);
      selector = i0;
      i0 = word_b;
      i0 = LoadU32(ctx, i0 + 4u);
      word_b = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
    }
    i0 = resume_code;
    i1 = kResumeInternString65;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = selector;
      i2 = word_b;
      i0 = InternString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeInternString65;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 48u);
      i0 = LoadU32(ctx, i0 + 4u);
      lex_state = i0;
    }
    i0 = resume_code;
    i1 = kResumeLookupKeyword66;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = lex_state;
      i2 = word_b;
      i0 = GetTableFieldByString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeLookupKeyword66;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      lex_state = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 8u);
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = selector;
      if (i0) {goto block_342;}
      i0 = lex_state;
      i1 = 1u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = lex_state;
      i1 = 1u;
      StoreU32(ctx, i0, i1);
      i0 = token;
      i0 = LoadU32(ctx, i0 + 16u);
      lex_state = i0;
      i0 = LoadU32(ctx, i0 + 68u);
      selector = i0;
      i0 = selector;
      i1 = lex_state;
      i1 = LoadU32(ctx, i1 + 64u);
      i0 = i0 < i1;
      lex_state = i0;
      if (i0) {goto block_342;}
    }
    i0 = resume_code;
    i1 = kResumeGrowTableSlot67;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      GrowTableSlot(ctx, i0);
      i0 = kResumeGrowTableSlot67;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
// merge point block_342 (wasm2c flattening boundary)
    block_342:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i0 = LoadU8(ctx, i0 + 6u);
      lex_state = i0;
      if (i0) {
        i0 = lex_state;
        i1 = 256u;
        i0 |= i1;
        token = i0;
        goto running_exit;
      }
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = kTokenName;
      token = i0;
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0);
    token = i0;
    i1 = 1u;
    i0 -= i1;
    selector = i0;
    i0 = seminfo;
    i1 = selector;
    StoreU32(ctx, i0, i1);
    i0 = token;
    if (i0) {
      i0 = seminfo;
      token = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      seminfo = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = lex_state;
      i1 = seminfo;
      i1 = LoadU8(ctx, i1);
      StoreU32(ctx, i0, i1);
      i0 = word_b;
      token = i0;
      goto running_exit;
    }
  }
  i0 = resume_code;
  i1 = kResumeNextChar68;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeNextChar68;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    seminfo = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lex_state;
    i1 = seminfo;
    StoreU32(ctx, i0, i1);
    i0 = word_b;
    token = i0;
    goto running_exit;
  }
// merge point block_7 (wasm2c flattening boundary)
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 60u);
    word_b = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    token = i0;
    i1 = 1u;
    i0 += i1;
    word_a = i0;
    i1 = word_b;
    i1 = LoadU32(ctx, i1 + 8u);
    word_d = i1;
    i0 = i0 <= i1;
    selector = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = selector;
    if (i0) {
      i0 = word_b;
      i0 = LoadU32(ctx, i0);
      word_d = i0;
      goto block_352;
    }
    i0 = word_d;
    i1 = kMaxBufferSize;
    i0 = i0 >= i1;
    token = i0;
  }
  i0 = token;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = frame_base;
      i1 = 208u;
      i0 += i1;
      i1 = lex_state;
      i1 = LoadU32(ctx, i1 + 64u);
      i2 = 16u;
      i1 += i2;
      i2 = kSourceIdBytes;
      FormatSourceId(ctx, i0, i1, i2);
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 4u);
      selector = i0;
      i0 = frame_base;
// pool text 175315: lexical element too long
      i1 = kMsgElementTooLong;
      StoreU32(ctx, i0 + 200u, i1);
      i0 = frame_base;
      i1 = selector;
      StoreU32(ctx, i0 + 196u, i1);
      i0 = frame_base;
      i1 = frame_base;
      i2 = 208u;
      i1 += i2;
      StoreU32(ctx, i0 + 192u, i1);
      i0 = frame_base;
      i1 = 192u;
      i0 += i1;
      selector = i0;
    }
    i0 = resume_code;
    i1 = kResumeFormatMessage69;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
// pool text 175292: %s:%d: %s
      i1 = kMsgAddInfoFmt;
      i2 = selector;
      i0 = FormatPushString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeFormatMessage69;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      token = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
    }
    i0 = resume_code;
    i1 = kResumeThrowSyntax70;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = 3u;
      ThrowScriptError(ctx, i0, i1);
      i0 = kResumeThrowSyntax70;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = word_d;
    } else {
      i0 = word_b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    word_d = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_d;
    i1 = 1u;
    i0 <<= (i1 & 31);
    selector = i0;
    i1 = kU32MinusTwo;
    i0 = i0 != i1;
    word_a = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 52u);
    token = i0;
  }
  i0 = word_a;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = word_b;
      i0 = LoadU32(ctx, i0);
      word_a = i0;
    }
    i0 = resume_code;
    i1 = kResumeReallocateBuffer71;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = word_a;
      i2 = word_d;
      i3 = selector;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      call_result = i0;
      i0 = kResumeReallocateBuffer71;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_d = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_363;}
  }
  i0 = resume_code;
  i1 = kResumeBlockTooBig72;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = RaiseBlockTooBig(ctx, i0);
    call_result = i0;
    i0 = kResumeBlockTooBig72;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    word_d = i0;
  }
// merge point block_363 (wasm2c flattening boundary)
  block_363:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    i1 = selector;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = word_b;
    i1 = word_d;
    StoreU32(ctx, i0, i1);
    i0 = word_b;
    i0 = LoadU32(ctx, i0 + 4u);
    token = i0;
    i1 = 1u;
    i0 += i1;
    word_a = i0;
  }
// merge point block_352 (wasm2c flattening boundary)
  block_352:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    i1 = word_a;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = token;
    i1 = word_d;
    i0 += i1;
    i1 = 46u;
    StoreU8(ctx, i0, i1);
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    token = i0;
    i0 = LoadU32(ctx, i0);
    word_b = i0;
    i1 = 1u;
    i0 -= i1;
    selector = i0;
    i0 = token;
    i1 = selector;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    selector = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    if (i0) {
      i0 = token;
      i1 = token;
      i1 = LoadU32(ctx, i1 + 4u);
      token = i1;
      i2 = 1u;
      i1 += i2;
      word_a = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = token;
      i0 = LoadU8(ctx, i0);
      goto block_370;
    }
  }
  i0 = resume_code;
  i1 = kResumeNextChar73;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeNextChar73;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = word_b;
  }
// merge point block_370 (wasm2c flattening boundary)
  block_370:;
  word_b = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = selector;
    i1 = word_b;
    StoreU32(ctx, i0, i1);
    i0 = word_b;
    i1 = 255u;
    i0 &= i1;
    token = i0;
    i1 = 63u;
    i0 = i0 > i1;
    selector = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = selector;
    if (i0) {goto block_375;}
    j0 = 1ull;
    i1 = token;
    j1 = (uint64_t)(i1);
    j0 <<= (j1 & 63);
    j1 = 70368744177665ull;
    j0 &= j1;
    i0 = !(j0);
    token = i0;
    if (i0) {goto block_375;}
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 60u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    word_a = i0;
    i1 = 1u;
    i0 += i1;
    word_d = i0;
    i0 = word_d;
    i1 = seminfo;
    i1 = LoadU32(ctx, i1 + 8u);
    selector = i1;
    i0 = i0 <= i1;
    token = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = token;
    if (i0) {
      i0 = seminfo;
      i0 = LoadU32(ctx, i0);
      selector = i0;
      goto block_377;
    }
    i0 = selector;
    i1 = kMaxBufferSize;
    i0 = i0 >= i1;
    token = i0;
  }
  i0 = token;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = frame_base;
      i1 = 208u;
      i0 += i1;
      i1 = lex_state;
      i1 = LoadU32(ctx, i1 + 64u);
      i2 = 16u;
      i1 += i2;
      i2 = kSourceIdBytes;
      FormatSourceId(ctx, i0, i1, i2);
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 4u);
      selector = i0;
      i0 = frame_base;
// pool text 175315: lexical element too long
      i1 = kMsgElementTooLong;
      StoreU32(ctx, i0 + 184u, i1);
      i0 = frame_base;
      i1 = selector;
      StoreU32(ctx, i0 + 180u, i1);
      i0 = frame_base;
      i1 = frame_base;
      i2 = 208u;
      i1 += i2;
      StoreU32(ctx, i0 + 176u, i1);
      i0 = frame_base;
      i1 = 176u;
      i0 += i1;
      selector = i0;
    }
    i0 = resume_code;
    i1 = kResumeFormatMessage74;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
// pool text 175292: %s:%d: %s
      i1 = kMsgAddInfoFmt;
      i2 = selector;
      i0 = FormatPushString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeFormatMessage74;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      token = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
    }
    i0 = resume_code;
    i1 = kResumeThrowSyntax75;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = 3u;
      ThrowScriptError(ctx, i0, i1);
      i0 = kResumeThrowSyntax75;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = selector;
    } else {
      i0 = seminfo;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    selector = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = selector;
    i1 = 1u;
    i0 <<= (i1 & 31);
    word_a = i0;
    i1 = kU32MinusTwo;
    i0 = i0 != i1;
    word_c = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 52u);
    token = i0;
  }
  i0 = word_c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = seminfo;
      i0 = LoadU32(ctx, i0);
      word_c = i0;
    }
    i0 = resume_code;
    i1 = kResumeReallocateBuffer76;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = word_c;
      i2 = selector;
      i3 = word_a;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      call_result = i0;
      i0 = kResumeReallocateBuffer76;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_388;}
  }
  i0 = resume_code;
  i1 = kResumeBlockTooBig77;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = RaiseBlockTooBig(ctx, i0);
    call_result = i0;
    i0 = kResumeBlockTooBig77;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    selector = i0;
  }
// merge point block_388 (wasm2c flattening boundary)
  block_388:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = seminfo;
    i1 = word_a;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = seminfo;
    i1 = selector;
    StoreU32(ctx, i0, i1);
    i0 = seminfo;
    i0 = LoadU32(ctx, i0 + 4u);
    word_a = i0;
    i1 = 1u;
    i0 += i1;
    word_d = i0;
  }
// merge point block_377 (wasm2c flattening boundary)
  block_377:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = seminfo;
    i1 = word_d;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = selector;
    i1 = word_a;
    i0 += i1;
    i1 = word_b;
    StoreU8(ctx, i0, i1);
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0);
    token = i0;
    i1 = 1u;
    i0 -= i1;
    word_b = i0;
    i0 = seminfo;
    i1 = word_b;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    selector = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = token;
    if (i0) {
      i0 = seminfo;
      token = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      seminfo = i0;
      i1 = 1u;
      i0 += i1;
      word_b = i0;
      i0 = token;
      i1 = word_b;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = seminfo;
      i0 = LoadU8(ctx, i0);
      goto block_395;
    }
  }
  i0 = resume_code;
  i1 = kResumeNextChar78;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeNextChar78;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = word_b;
  }
// merge point block_395 (wasm2c flattening boundary)
  block_395:;
  word_b = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = selector;
    i1 = word_b;
    StoreU32(ctx, i0, i1);
    i0 = kTokenConcat;
    token = i0;
    i0 = word_b;
    i1 = 255u;
    i0 &= i1;
    seminfo = i0;
    i1 = 63u;
    i0 = i0 > i1;
    if (i0) {goto running_exit;}
    j0 = 1ull;
    i1 = seminfo;
    j1 = (uint64_t)(i1);
    j0 <<= (j1 & 63);
    j1 = 70368744177665ull;
    j0 &= j1;
    i0 = !(j0);
    if (i0) {goto running_exit;}
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 60u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    word_a = i0;
    i1 = 1u;
    i0 += i1;
    word_d = i0;
    i0 = word_d;
    i1 = seminfo;
    i1 = LoadU32(ctx, i1 + 8u);
    selector = i1;
    i0 = i0 <= i1;
    token = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = token;
    if (i0) {
      i0 = seminfo;
      i0 = LoadU32(ctx, i0);
      selector = i0;
      goto block_400;
    }
    i0 = selector;
    i1 = kMaxBufferSize;
    i0 = i0 >= i1;
    token = i0;
  }
  i0 = token;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = frame_base;
      i1 = 208u;
      i0 += i1;
      i1 = lex_state;
      i1 = LoadU32(ctx, i1 + 64u);
      i2 = 16u;
      i1 += i2;
      i2 = kSourceIdBytes;
      FormatSourceId(ctx, i0, i1, i2);
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 4u);
      selector = i0;
      i0 = frame_base;
// pool text 175315: lexical element too long
      i1 = kMsgElementTooLong;
      StoreU32(ctx, i0 + 168u, i1);
      i0 = frame_base;
      i1 = selector;
      StoreU32(ctx, i0 + 164u, i1);
      i0 = frame_base;
      i1 = frame_base;
      i2 = 208u;
      i1 += i2;
      StoreU32(ctx, i0 + 160u, i1);
      i0 = frame_base;
      i1 = 160u;
      i0 += i1;
      selector = i0;
    }
    i0 = resume_code;
    i1 = kResumeFormatMessage79;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
// pool text 175292: %s:%d: %s
      i1 = kMsgAddInfoFmt;
      i2 = selector;
      i0 = FormatPushString(ctx, i0, i1, i2);
      call_result = i0;
      i0 = kResumeFormatMessage79;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      token = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 52u);
      token = i0;
    }
    i0 = resume_code;
    i1 = kResumeThrowSyntax80;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = 3u;
      ThrowScriptError(ctx, i0, i1);
      i0 = kResumeThrowSyntax80;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = selector;
    } else {
      i0 = seminfo;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    selector = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = selector;
    i1 = 1u;
    i0 <<= (i1 & 31);
    word_a = i0;
    i1 = kU32MinusTwo;
    i0 = i0 != i1;
    word_c = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 52u);
    token = i0;
  }
  i0 = word_c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = seminfo;
      i0 = LoadU32(ctx, i0);
      word_c = i0;
    }
    i0 = resume_code;
    i1 = kResumeReallocateBuffer81;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = word_c;
      i2 = selector;
      i3 = word_a;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      call_result = i0;
      i0 = kResumeReallocateBuffer81;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      selector = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_411;}
  }
  i0 = resume_code;
  i1 = kResumeBlockTooBig82;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = RaiseBlockTooBig(ctx, i0);
    call_result = i0;
    i0 = kResumeBlockTooBig82;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    selector = i0;
  }
// merge point block_411 (wasm2c flattening boundary)
  block_411:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = seminfo;
    i1 = word_a;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = seminfo;
    i1 = selector;
    StoreU32(ctx, i0, i1);
    i0 = seminfo;
    i0 = LoadU32(ctx, i0 + 4u);
    word_a = i0;
    i1 = 1u;
    i0 += i1;
    word_d = i0;
  }
// merge point block_400 (wasm2c flattening boundary)
  block_400:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = seminfo;
    i1 = word_d;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = selector;
    i1 = word_a;
    i0 += i1;
    i1 = word_b;
    StoreU8(ctx, i0, i1);
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0);
    token = i0;
    i1 = 1u;
    i0 -= i1;
    word_b = i0;
    i0 = seminfo;
    i1 = word_b;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    word_b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = token;
    if (i0) {
      i0 = seminfo;
      i1 = seminfo;
      i1 = LoadU32(ctx, i1 + 4u);
      lex_state = i1;
      i2 = 1u;
      i1 += i2;
      token = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = lex_state;
      i0 = LoadU8(ctx, i0);
      goto block_418;
    }
  }
  i0 = resume_code;
  i1 = kResumeNextChar83;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeNextChar83;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = lex_state;
  }
// merge point block_418 (wasm2c flattening boundary)
  block_418:;
  lex_state = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    i1 = lex_state;
    StoreU32(ctx, i0, i1);
    i0 = kTokenDots;
    token = i0;
    goto running_exit;
  }
// merge point block_375 (wasm2c flattening boundary)
  block_375:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 46u;
    token = i0;
    i0 = word_b;
    i1 = 48u;
    i0 -= i1;
    i1 = 9u;
    i0 = i0 > i1;
    word_b = i0;
    if (i0) {goto running_exit;}
  }
// merge point block_6 (wasm2c flattening boundary)
  block_6:;
  i0 = resume_code;
  i1 = kResumeReadNumeral84;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = lex_state;
    i1 = seminfo;
    ReadNumeral(ctx, i0, i1);
    i0 = kResumeReadNumeral84;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kTokenNumber;
    token = i0;
    goto running_exit;
  }
// merge point block_5 (wasm2c flattening boundary)
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0);
    token = i0;
    i1 = 1u;
    i0 -= i1;
    word_b = i0;
    i0 = seminfo;
    i1 = word_b;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    word_b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = token;
    if (i0) {
      i0 = seminfo;
      token = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      seminfo = i0;
      i1 = 1u;
      i0 += i1;
      selector = i0;
      i0 = token;
      i1 = selector;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = seminfo;
      i0 = LoadU8(ctx, i0);
      goto block_427;
    }
  }
  i0 = resume_code;
  i1 = kResumeNextChar85;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeNextChar85;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = seminfo;
  }
// merge point block_427 (wasm2c flattening boundary)
  block_427:;
  seminfo = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    i1 = seminfo;
    StoreU32(ctx, i0, i1);
    i0 = 126u;
    token = i0;
    i0 = seminfo;
    i1 = 61u;
    i0 = i0 != i1;
    if (i0) {goto running_exit;}
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    seminfo = i0;
    i0 = LoadU32(ctx, i0);
    token = i0;
    i1 = 1u;
    i0 -= i1;
    word_b = i0;
    i0 = seminfo;
    i1 = word_b;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    word_b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = token;
    if (i0) {
      i0 = seminfo;
      i1 = seminfo;
      i1 = LoadU32(ctx, i1 + 4u);
      lex_state = i1;
      i2 = 1u;
      i1 += i2;
      token = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = lex_state;
      i0 = LoadU8(ctx, i0);
      goto block_432;
    }
  }
  i0 = resume_code;
  i1 = kResumeNextChar86;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeNextChar86;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = lex_state;
  }
// merge point block_432 (wasm2c flattening boundary)
  block_432:;
  lex_state = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = word_b;
    i1 = lex_state;
    StoreU32(ctx, i0, i1);
    i0 = kTokenNe;
    token = i0;
  }
// merge point running_exit (wasm2c flattening boundary)
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = frame_base;
    i1 = kLocalFrameBytes;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = token;
    goto func_exit;
  }
  TrapUnreachable();
// merge point spill_frame (wasm2c flattening boundary)
  spill_frame:;
  call_result = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = call_result;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1727ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  call_result = i0;
  i1 = lex_state;
  StoreU32(ctx, i0, i1);
  i0 = call_result;
  i1 = seminfo;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = call_result;
  i1 = token;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = call_result;
  i1 = selector;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = call_result;
  i1 = word_a;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = call_result;
  i1 = word_b;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = call_result;
  i1 = word_c;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = call_result;
  i1 = word_d;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = call_result;
  i1 = frame_base;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = call_result;
  i1 = word_e;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = call_result;
  i1 = word_f;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1727SaveFrameBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}

}  // namespace artc
