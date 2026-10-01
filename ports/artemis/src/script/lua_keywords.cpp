#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

// The Lua reserved words the lexer's table holds, as pointers into the module's data
// section, listed in the order the table stores them: and, break, do, else, elseif,
// end, false, for, function, if, in, local, nil, not, or, repeat, return, then, true,
// until, while.
constexpr uint32_t kLuaKeywordAnd = 0x2AB8Bu;
constexpr uint32_t kLuaKeywordBreak = 0x2AB8Fu;
constexpr uint32_t kLuaKeywordDo = 0x2AB95u;
constexpr uint32_t kLuaKeywordElse = 0x2AB98u;
constexpr uint32_t kLuaKeywordElseif = 0x2AB9Du;
constexpr uint32_t kLuaKeywordEnd = 0x2ABA4u;
constexpr uint32_t kLuaKeywordFalse = 0x2ABA8u;
constexpr uint32_t kLuaKeywordFor = 0x2ABAEu;
constexpr uint32_t kLuaKeywordFunction = 0x2ABB2u;
constexpr uint32_t kLuaKeywordIf = 0x2ABBBu;
constexpr uint32_t kLuaKeywordIn = 0x2ABBEu;
constexpr uint32_t kLuaKeywordLocal = 0x2ABC1u;
constexpr uint32_t kLuaKeywordNil = 0x2ABC7u;
constexpr uint32_t kLuaKeywordNot = 0x2ABCBu;
constexpr uint32_t kLuaKeywordOr = 0x2ABCFu;
constexpr uint32_t kLuaKeywordRepeat = 0x2ABD2u;
constexpr uint32_t kLuaKeywordReturn = 0x2ABD9u;
constexpr uint32_t kLuaKeywordThen = 0x2ABE0u;
constexpr uint32_t kLuaKeywordTrue = 0x2ABE5u;
constexpr uint32_t kLuaKeywordUntil = 0x2ABEAu;
constexpr uint32_t kLuaKeywordWhile = 0x2ABF0u;

}  // namespace

namespace LuaKeywords {

// Builds the reserved-word table the lexer matches against: every spelling below is
// interned once, and the entry the intern returns takes its 1-based reserved index at
// +6 and the 32 bit in its +5 flag byte. Interning can run guest code, so the walk is
// suspendable: a rewinding pass restores the state object and the current entry, and
// the one site whose resume code matches replays its intern call.
void Init(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 8u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the intern sites below spill, one per reserved word in table order.
  uint32_t const kAndIntern = 0u;
  uint32_t const kBreakIntern = 1u;
  uint32_t const kDoIntern = 2u;
  uint32_t const kElseIntern = 3u;
  uint32_t const kElseifIntern = 4u;
  uint32_t const kEndIntern = 5u;
  uint32_t const kFalseIntern = 6u;
  uint32_t const kForIntern = 7u;
  uint32_t const kFunctionIntern = 8u;
  uint32_t const kIfIntern = 9u;
  uint32_t const kInIntern = 10u;
  uint32_t const kLocalIntern = 11u;
  uint32_t const kNilIntern = 12u;
  uint32_t const kNotIntern = 13u;
  uint32_t const kOrIntern = 14u;
  uint32_t const kRepeatIntern = 15u;
  uint32_t const kReturnIntern = 16u;
  uint32_t const kThenIntern = 17u;
  uint32_t const kTrueIntern = 18u;
  uint32_t const kUntilIntern = 19u;
  uint32_t const kWhileIntern = 20u;
  uint32_t entry = 0u;
  uint32_t call_result = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the eight-byte frame the suspend path saved and restore the
    // state object and the entry the last intern returned, in that order.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    entry = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the intern call that was in flight. The two pops stay
    // separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAndIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordAnd, 3u);
    resume_code = kAndIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 1u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kBreakIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordBreak, 5u);
    resume_code = kBreakIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 2u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDoIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordDo, 2u);
    resume_code = kDoIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 3u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kElseIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordElse, 4u);
    resume_code = kElseIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 4u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kElseifIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordElseif, 6u);
    resume_code = kElseifIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 5u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEndIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordEnd, 3u);
    resume_code = kEndIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 6u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFalseIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordFalse, 5u);
    resume_code = kFalseIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 7u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kForIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordFor, 3u);
    resume_code = kForIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 8u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFunctionIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordFunction, 8u);
    resume_code = kFunctionIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 9u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kIfIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordIf, 2u);
    resume_code = kIfIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 10u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordIn, 2u);
    resume_code = kInIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 11u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kLocalIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordLocal, 5u);
    resume_code = kLocalIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 12u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNilIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordNil, 3u);
    resume_code = kNilIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 13u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNotIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordNot, 3u);
    resume_code = kNotIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 14u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kOrIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordOr, 2u);
    resume_code = kOrIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 15u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRepeatIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordRepeat, 6u);
    resume_code = kRepeatIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 16u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReturnIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordReturn, 6u);
    resume_code = kReturnIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 17u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThenIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordThen, 4u);
    resume_code = kThenIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 18u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTrueIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordTrue, 4u);
    resume_code = kTrueIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 19u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kUntilIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordUntil, 5u);
    resume_code = kUntilIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 20u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kWhileIntern) : 1u) {
    call_result = InternString(ctx, state, kLuaKeywordWhile, 5u);
    resume_code = kWhileIntern;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    entry = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, entry + 6u, 21u);
    StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) | 32u);
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: push the code naming the intern in flight, step past it, then push the
    // eight-byte frame a rewinding pass reads back -- the state object and the entry
    // the last intern returned, in that order. The stack pointer lives in the word the
    // frame pointer itself names, so every push is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, entry);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace LuaKeywords

}  // namespace artc
