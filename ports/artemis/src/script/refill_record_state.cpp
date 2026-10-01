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

// RefillRecordState runs one lexer step: it takes the lexer state and the
// semantic slot the caller wants the token payload in, reads the next token out
// of the input stream and answers with its token code. Whitespace and comments
// are skipped here; lexical errors are raised as syntax errors carrying the
// source position. Suspendable.

uint32_t RefillRecordState(EngineContext* ctx, uint32_t lex_state, uint32_t seminfo) {
  EnterFrame();
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
  constexpr uint32_t kSaveFrameBytes = 44u;
  constexpr uint32_t kResumeWordBytes = 4u;
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
  constexpr uint32_t kResumeStepCall0 = 0u;
  constexpr uint32_t kResumeStepNextChar1 = 1u;
  constexpr uint32_t kResumeStepNextChar2 = 2u;
  constexpr uint32_t kResumeStepSkipSep3 = 3u;
  constexpr uint32_t kResumeStepNextChar4 = 4u;
  constexpr uint32_t kResumeStepSkipSep5 = 5u;
  constexpr uint32_t kResumeStepReadLongString6 = 6u;
  constexpr uint32_t kResumeStepLexError7 = 7u;
  constexpr uint32_t kResumeStepNextChar8 = 8u;
  constexpr uint32_t kResumeStepNextChar9 = 9u;
  constexpr uint32_t kResumeStepNextChar10 = 10u;
  constexpr uint32_t kResumeStepNextChar11 = 11u;
  constexpr uint32_t kResumeStepNextChar12 = 12u;
  constexpr uint32_t kResumeStepNextChar13 = 13u;
  constexpr uint32_t kResumeStepFormatMessage14 = 14u;
  constexpr uint32_t kResumeStepThrowSyntax15 = 15u;
  constexpr uint32_t kResumeStepReallocateBuffer16 = 16u;
  constexpr uint32_t kResumeStepBlockTooBig17 = 17u;
  constexpr uint32_t kResumeStepNextChar18 = 18u;
  constexpr uint32_t kResumeStepFormatMessage19 = 19u;
  constexpr uint32_t kResumeStepFormatMessage20 = 20u;
  constexpr uint32_t kResumeStepThrowSyntax21 = 21u;
  constexpr uint32_t kResumeStepLexError22 = 22u;
  constexpr uint32_t kResumeStepFormatMessage23 = 23u;
  constexpr uint32_t kResumeStepThrowSyntax24 = 24u;
  constexpr uint32_t kResumeStepReallocateBuffer25 = 25u;
  constexpr uint32_t kResumeStepBlockTooBig26 = 26u;
  constexpr uint32_t kResumeStepNextChar27 = 27u;
  constexpr uint32_t kResumeStepNextChar28 = 28u;
  constexpr uint32_t kResumeStepFormatMessage29 = 29u;
  constexpr uint32_t kResumeStepThrowSyntax30 = 30u;
  constexpr uint32_t kResumeStepReallocateBuffer31 = 31u;
  constexpr uint32_t kResumeStepBlockTooBig32 = 32u;
  constexpr uint32_t kResumeStepInclineNumber33 = 33u;
  constexpr uint32_t kResumeStepNextChar34 = 34u;
  constexpr uint32_t kResumeStepLexError35 = 35u;
  constexpr uint32_t kResumeStepFormatMessage36 = 36u;
  constexpr uint32_t kResumeStepThrowSyntax37 = 37u;
  constexpr uint32_t kResumeStepReallocateBuffer38 = 38u;
  constexpr uint32_t kResumeStepBlockTooBig39 = 39u;
  constexpr uint32_t kResumeStepFormatMessage40 = 40u;
  constexpr uint32_t kResumeStepThrowSyntax41 = 41u;
  constexpr uint32_t kResumeStepReallocateBuffer42 = 42u;
  constexpr uint32_t kResumeStepBlockTooBig43 = 43u;
  constexpr uint32_t kResumeStepNextChar44 = 44u;
  constexpr uint32_t kResumeStepFormatMessage45 = 45u;
  constexpr uint32_t kResumeStepThrowSyntax46 = 46u;
  constexpr uint32_t kResumeStepReallocateBuffer47 = 47u;
  constexpr uint32_t kResumeStepBlockTooBig48 = 48u;
  constexpr uint32_t kResumeStepNextChar49 = 49u;
  constexpr uint32_t kResumeStepFormatMessage50 = 50u;
  constexpr uint32_t kResumeStepThrowSyntax51 = 51u;
  constexpr uint32_t kResumeStepReallocateBuffer52 = 52u;
  constexpr uint32_t kResumeStepBlockTooBig53 = 53u;
  constexpr uint32_t kResumeStepNextChar54 = 54u;
  constexpr uint32_t kResumeStepInternString55 = 55u;
  constexpr uint32_t kResumeStepLookupKeyword56 = 56u;
  constexpr uint32_t kResumeStepGrowTableSlot57 = 57u;
  constexpr uint32_t kResumeStepReadLongString58 = 58u;
  constexpr uint32_t kResumeStepNextChar59 = 59u;
  constexpr uint32_t kResumeStepFormatMessage60 = 60u;
  constexpr uint32_t kResumeStepThrowSyntax61 = 61u;
  constexpr uint32_t kResumeStepReallocateBuffer62 = 62u;
  constexpr uint32_t kResumeStepBlockTooBig63 = 63u;
  constexpr uint32_t kResumeStepNextChar64 = 64u;
  constexpr uint32_t kResumeStepInternString65 = 65u;
  constexpr uint32_t kResumeStepLookupKeyword66 = 66u;
  constexpr uint32_t kResumeStepGrowTableSlot67 = 67u;
  constexpr uint32_t kResumeStepNextChar68 = 68u;
  constexpr uint32_t kResumeStepFormatMessage69 = 69u;
  constexpr uint32_t kResumeStepThrowSyntax70 = 70u;
  constexpr uint32_t kResumeStepReallocateBuffer71 = 71u;
  constexpr uint32_t kResumeStepBlockTooBig72 = 72u;
  constexpr uint32_t kResumeStepNextChar73 = 73u;
  constexpr uint32_t kResumeStepFormatMessage74 = 74u;
  constexpr uint32_t kResumeStepThrowSyntax75 = 75u;
  constexpr uint32_t kResumeStepReallocateBuffer76 = 76u;
  constexpr uint32_t kResumeStepBlockTooBig77 = 77u;
  constexpr uint32_t kResumeStepNextChar78 = 78u;
  constexpr uint32_t kResumeStepFormatMessage79 = 79u;
  constexpr uint32_t kResumeStepThrowSyntax80 = 80u;
  constexpr uint32_t kResumeStepReallocateBuffer81 = 81u;
  constexpr uint32_t kResumeStepBlockTooBig82 = 82u;
  constexpr uint32_t kResumeStepNextChar83 = 83u;
  constexpr uint32_t kResumeStepReadNumeral84 = 84u;
  constexpr uint32_t kResumeStepNextChar85 = 85u;
  constexpr uint32_t kResumeStepNextChar86 = 86u;
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
    i2 = kSaveFrameBytes;
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
    i2 = kResumeWordBytes;
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
// Merge point dispatch_loop.
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
        case 0: goto exit_path;
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
// Merge point block_20.
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
      i0 = kResumeStepCall0;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto dispatch_loop;}
// Merge point block_18.
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
    i1 = kResumeStepNextChar1;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar1;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = word_b;
    }
// Merge point block_23.
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
      if (i0) {goto exit_path;}
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
    i1 = kResumeStepNextChar2;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar2;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = word_b;
    }
// Merge point block_28.
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
      i1 = kResumeStepSkipSep3;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = lex_state;
        i0 = SkipSep(ctx, i0);
        call_result = i0;
        i0 = kResumeStepSkipSep3;
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
// Merge point loop_37.
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
        i1 = kResumeStepNextChar4;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeStepNextChar4;
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
// Merge point block_17.
    block_17:;
    i0 = word_b;
    i1 = resume_code;
    i2 = kResumeStepSkipSep5;
    i1 = i1 == i2;
    i2 = 1u;
    i3 = ResumePhase(ctx);
    i1 = i3 ? i1 : i2;
    if (i1) {
      i1 = lex_state;
      i1 = SkipSep(ctx, i1);
      call_result = i1;
      i1 = kResumeStepSkipSep5;
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
      i1 = kResumeStepReadLongString6;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = lex_state;
        i1 = seminfo;
        i2 = token;
        ReadLongString(ctx, i0, i1, i2);
        i0 = kResumeStepReadLongString6;
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
        goto exit_path;
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
        goto exit_path;
      }
    }
    i0 = resume_code;
    i1 = kResumeStepLexError7;
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
      i0 = kResumeStepLexError7;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
// Merge point block_16.
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
    i1 = kResumeStepNextChar8;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar8;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = seminfo;
    }
// Merge point block_53.
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
      if (i0) {goto exit_path;}
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
    i1 = kResumeStepNextChar9;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar9;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = lex_state;
    }
// Merge point block_58.
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
      goto exit_path;
    }
// Merge point block_15.
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
    i1 = kResumeStepNextChar10;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar10;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = seminfo;
    }
// Merge point block_64.
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
      if (i0) {goto exit_path;}
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
    i1 = kResumeStepNextChar11;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar11;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = lex_state;
    }
// Merge point block_69.
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
      goto exit_path;
    }
// Merge point block_14.
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
    i1 = kResumeStepNextChar12;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar12;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = seminfo;
    }
// Merge point block_75.
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
      if (i0) {goto exit_path;}
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
    i1 = kResumeStepNextChar13;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = seminfo;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar13;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = lex_state;
    }
// Merge point block_80.
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
      goto exit_path;
    }
// Merge point block_13.
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
      i1 = kResumeStepFormatMessage14;
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
        i0 = kResumeStepFormatMessage14;
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
      i1 = kResumeStepThrowSyntax15;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
        i1 = 3u;
        ThrowScriptError(ctx, i0, i1);
        i0 = kResumeStepThrowSyntax15;
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
      i1 = kResumeStepReallocateBuffer16;
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
        i0 = kResumeStepReallocateBuffer16;
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
    i1 = kResumeStepBlockTooBig17;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = selector;
      i0 = RaiseBlockTooBig(ctx, i0);
      call_result = i0;
      i0 = kResumeStepBlockTooBig17;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_a = i0;
    }
// Merge point block_97.
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
// Merge point block_86.
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
    i1 = kResumeStepNextChar18;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar18;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = token;
    }
// Merge point block_104.
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
// Merge point loop_111.
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
// Merge point block_118.
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
        i1 = kResumeStepFormatMessage19;
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
          i0 = kResumeStepFormatMessage19;
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
        i1 = kResumeStepFormatMessage20;
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
          i0 = kResumeStepFormatMessage20;
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
        i1 = kResumeStepThrowSyntax21;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i1 = 3u;
          ThrowScriptError(ctx, i0, i1);
          i0 = kResumeStepThrowSyntax21;
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
// Merge point block_116.
        block_116:;
        i0 = resume_code;
        i1 = kResumeStepLexError22;
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
          i0 = kResumeStepLexError22;
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
// Merge point block_115.
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
// Merge point block_114.
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
          i1 = kResumeStepFormatMessage23;
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
            i0 = kResumeStepFormatMessage23;
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
          i1 = kResumeStepThrowSyntax24;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeStepThrowSyntax24;
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
          i1 = kResumeStepReallocateBuffer25;
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
            i0 = kResumeStepReallocateBuffer25;
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
        i1 = kResumeStepBlockTooBig26;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeStepBlockTooBig26;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_c = i0;
        }
// Merge point block_140.
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
// Merge point block_129.
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
        i1 = kResumeStepNextChar27;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeStepNextChar27;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
        } else {
          i0 = token;
        }
// Merge point block_147.
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
// Merge point block_113.
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
        i1 = kResumeStepNextChar28;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeStepNextChar28;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
        } else {
          i0 = token;
        }
// Merge point block_153.
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
// Merge point block_170.
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
// Merge point block_169.
          block_169:;
          i0 = 12u;
          word_a = i0;
          goto block_159;
// Merge point block_168.
          block_168:;
          i0 = 10u;
          word_a = i0;
          goto block_159;
// Merge point block_167.
          block_167:;
          i0 = 13u;
          word_a = i0;
          goto block_159;
// Merge point block_166.
          block_166:;
          i0 = 9u;
          word_a = i0;
          goto block_159;
// Merge point block_165.
          block_165:;
          i0 = 11u;
          word_a = i0;
          goto block_159;
// Merge point block_164.
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
          i1 = kResumeStepFormatMessage29;
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
            i0 = kResumeStepFormatMessage29;
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
          i1 = kResumeStepThrowSyntax30;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeStepThrowSyntax30;
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
          i1 = kResumeStepReallocateBuffer31;
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
            i0 = kResumeStepReallocateBuffer31;
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
        i1 = kResumeStepBlockTooBig32;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = word_a;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeStepBlockTooBig32;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          selector = i0;
        }
// Merge point block_182.
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
// Merge point block_171.
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
        i1 = kResumeStepInclineNumber33;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = lex_state;
          InclineNumber(ctx, i0);
          i0 = kResumeStepInclineNumber33;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
// Merge point block_162.
        block_162:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = lex_state;
          i0 = LoadU32(ctx, i0);
          token = i0;
          goto block_112;
        }
// Merge point block_161.
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
// Merge point loop_192.
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
          i1 = kResumeStepNextChar34;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_a;
            i0 = NextCharSlow(ctx, i0);
            call_result = i0;
            i0 = kResumeStepNextChar34;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = call_result;
          } else {
            i0 = token;
          }
// Merge point block_194.
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
          i1 = kResumeStepLexError35;
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
            i0 = kResumeStepLexError35;
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
          i1 = kResumeStepFormatMessage36;
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
            i0 = kResumeStepFormatMessage36;
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
          i1 = kResumeStepThrowSyntax37;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = word_a;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeStepThrowSyntax37;
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
          i1 = kResumeStepReallocateBuffer38;
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
            i0 = kResumeStepReallocateBuffer38;
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
        i1 = kResumeStepBlockTooBig39;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = word_d;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeStepBlockTooBig39;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_a = i0;
        }
// Merge point block_213.
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
// Merge point block_203.
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
// Merge point block_160.
        block_160:;
        i0 = word_a;
        i1 = 8u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        word_a = i0;
// Merge point block_159.
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
          i1 = kResumeStepFormatMessage40;
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
            i0 = kResumeStepFormatMessage40;
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
          i1 = kResumeStepThrowSyntax41;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeStepThrowSyntax41;
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
          i1 = kResumeStepReallocateBuffer42;
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
            i0 = kResumeStepReallocateBuffer42;
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
        i1 = kResumeStepBlockTooBig43;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeStepBlockTooBig43;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_c = i0;
        }
// Merge point block_234.
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
// Merge point block_223.
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
        i1 = kResumeStepNextChar44;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeStepNextChar44;
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
// Merge point block_158.
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
          i1 = kResumeStepFormatMessage45;
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
            i0 = kResumeStepFormatMessage45;
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
          i1 = kResumeStepThrowSyntax46;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = selector;
            i1 = 3u;
            ThrowScriptError(ctx, i0, i1);
            i0 = kResumeStepThrowSyntax46;
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
          i1 = kResumeStepReallocateBuffer47;
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
            i0 = kResumeStepReallocateBuffer47;
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
        i1 = kResumeStepBlockTooBig48;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i0 = RaiseBlockTooBig(ctx, i0);
          call_result = i0;
          i0 = kResumeStepBlockTooBig48;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = call_result;
          word_c = i0;
        }
// Merge point block_256.
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
// Merge point block_245.
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
        i1 = kResumeStepNextChar49;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = token;
          i0 = NextCharSlow(ctx, i0);
          call_result = i0;
          i0 = kResumeStepNextChar49;
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
// Merge point block_112.
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
      i1 = kResumeStepFormatMessage50;
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
        i0 = kResumeStepFormatMessage50;
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
      i1 = kResumeStepThrowSyntax51;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = selector;
        i1 = 3u;
        ThrowScriptError(ctx, i0, i1);
        i0 = kResumeStepThrowSyntax51;
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
      i1 = kResumeStepReallocateBuffer52;
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
        i0 = kResumeStepReallocateBuffer52;
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
    i1 = kResumeStepBlockTooBig53;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = selector;
      i0 = RaiseBlockTooBig(ctx, i0);
      call_result = i0;
      i0 = kResumeStepBlockTooBig53;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
      word_a = i0;
    }
// Merge point block_279.
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
// Merge point block_268.
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
    i1 = kResumeStepNextChar54;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i0 = NextCharSlow(ctx, i0);
      call_result = i0;
      i0 = kResumeStepNextChar54;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = call_result;
    } else {
      i0 = token;
    }
// Merge point block_286.
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
    i1 = kResumeStepInternString55;
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
      i0 = kResumeStepInternString55;
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
    i1 = kResumeStepLookupKeyword56;
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
      i0 = kResumeStepLookupKeyword56;
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
    i1 = kResumeStepGrowTableSlot57;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      GrowTableSlot(ctx, i0);
      i0 = kResumeStepGrowTableSlot57;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
// Merge point block_295.
    block_295:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = kTokenString;
      token = i0;
      goto exit_path;
    }
// Merge point block_12.
    block_12:;
    i0 = resume_code;
    i1 = kResumeStepReadLongString58;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = 0u;
      i2 = token;
      ReadLongString(ctx, i0, i1, i2);
      i0 = kResumeStepReadLongString58;
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
// Merge point block_11.
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
// Merge point block_10.
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
      i1 = kResumeStepNextChar59;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = token;
        i0 = NextCharSlow(ctx, i0);
        call_result = i0;
        i0 = kResumeStepNextChar59;
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
// Merge point loop_312.
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
        i1 = kResumeStepFormatMessage60;
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
          i0 = kResumeStepFormatMessage60;
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
        i1 = kResumeStepThrowSyntax61;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = selector;
          i1 = 3u;
          ThrowScriptError(ctx, i0, i1);
          i0 = kResumeStepThrowSyntax61;
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
        i1 = kResumeStepReallocateBuffer62;
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
          i0 = kResumeStepReallocateBuffer62;
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
      i1 = kResumeStepBlockTooBig63;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = word_a;
        i0 = RaiseBlockTooBig(ctx, i0);
        call_result = i0;
        i0 = kResumeStepBlockTooBig63;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
        selector = i0;
      }
// Merge point block_325.
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
// Merge point block_314.
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
      i1 = kResumeStepNextChar64;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = token;
        i0 = NextCharSlow(ctx, i0);
        call_result = i0;
        i0 = kResumeStepNextChar64;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = call_result;
      } else {
        i0 = word_b;
      }
// Merge point block_332.
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
    i1 = kResumeStepInternString65;
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
      i0 = kResumeStepInternString65;
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
    i1 = kResumeStepLookupKeyword66;
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
      i0 = kResumeStepLookupKeyword66;
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
    i1 = kResumeStepGrowTableSlot67;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      GrowTableSlot(ctx, i0);
      i0 = kResumeStepGrowTableSlot67;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
// Merge point block_342.
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
        goto exit_path;
      }
      i0 = seminfo;
      i1 = word_b;
      StoreU32(ctx, i0, i1);
      i0 = kTokenName;
      token = i0;
      goto exit_path;
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
      goto exit_path;
    }
  }
  i0 = resume_code;
  i1 = kResumeStepNextChar68;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeStepNextChar68;
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
    goto exit_path;
  }
// Merge point block_7.
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
    i1 = kResumeStepFormatMessage69;
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
      i0 = kResumeStepFormatMessage69;
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
    i1 = kResumeStepThrowSyntax70;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = 3u;
      ThrowScriptError(ctx, i0, i1);
      i0 = kResumeStepThrowSyntax70;
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
    i1 = kResumeStepReallocateBuffer71;
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
      i0 = kResumeStepReallocateBuffer71;
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
  i1 = kResumeStepBlockTooBig72;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = RaiseBlockTooBig(ctx, i0);
    call_result = i0;
    i0 = kResumeStepBlockTooBig72;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    word_d = i0;
  }
// Merge point block_363.
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
// Merge point block_352.
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
  i1 = kResumeStepNextChar73;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeStepNextChar73;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = word_b;
  }
// Merge point block_370.
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
    i1 = kResumeStepFormatMessage74;
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
      i0 = kResumeStepFormatMessage74;
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
    i1 = kResumeStepThrowSyntax75;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = 3u;
      ThrowScriptError(ctx, i0, i1);
      i0 = kResumeStepThrowSyntax75;
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
    i1 = kResumeStepReallocateBuffer76;
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
      i0 = kResumeStepReallocateBuffer76;
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
  i1 = kResumeStepBlockTooBig77;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = RaiseBlockTooBig(ctx, i0);
    call_result = i0;
    i0 = kResumeStepBlockTooBig77;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    selector = i0;
  }
// Merge point block_388.
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
// Merge point block_377.
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
  i1 = kResumeStepNextChar78;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeStepNextChar78;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = word_b;
  }
// Merge point block_395.
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
    if (i0) {goto exit_path;}
    j0 = 1ull;
    i1 = seminfo;
    j1 = (uint64_t)(i1);
    j0 <<= (j1 & 63);
    j1 = 70368744177665ull;
    j0 &= j1;
    i0 = !(j0);
    if (i0) {goto exit_path;}
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
    i1 = kResumeStepFormatMessage79;
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
      i0 = kResumeStepFormatMessage79;
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
    i1 = kResumeStepThrowSyntax80;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = token;
      i1 = 3u;
      ThrowScriptError(ctx, i0, i1);
      i0 = kResumeStepThrowSyntax80;
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
    i1 = kResumeStepReallocateBuffer81;
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
      i0 = kResumeStepReallocateBuffer81;
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
  i1 = kResumeStepBlockTooBig82;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = token;
    i0 = RaiseBlockTooBig(ctx, i0);
    call_result = i0;
    i0 = kResumeStepBlockTooBig82;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
    selector = i0;
  }
// Merge point block_411.
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
// Merge point block_400.
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
  i1 = kResumeStepNextChar83;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeStepNextChar83;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = lex_state;
  }
// Merge point block_418.
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
    goto exit_path;
  }
// Merge point block_375.
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
    if (i0) {goto exit_path;}
  }
// Merge point block_6.
  block_6:;
  i0 = resume_code;
  i1 = kResumeStepReadNumeral84;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = lex_state;
    i1 = seminfo;
    ReadNumeral(ctx, i0, i1);
    i0 = kResumeStepReadNumeral84;
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
    goto exit_path;
  }
// Merge point block_5.
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
  i1 = kResumeStepNextChar85;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeStepNextChar85;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = seminfo;
  }
// Merge point block_427.
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
    if (i0) {goto exit_path;}
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
  i1 = kResumeStepNextChar86;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = seminfo;
    i0 = NextCharSlow(ctx, i0);
    call_result = i0;
    i0 = kResumeStepNextChar86;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = call_result;
  } else {
    i0 = lex_state;
  }
// Merge point block_432.
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
// Merge point exit_path.
  exit_path:;
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
// Merge point spill_frame.
  spill_frame:;
  call_result = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = call_result;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kResumeWordBytes;
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
  i2 = kSaveFrameBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}  // namespace artc

}  // namespace artc
