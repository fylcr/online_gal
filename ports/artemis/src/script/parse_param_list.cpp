#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void PatchUpvalueJumpTarget(EngineContext* ctx, uint32_t a);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void DeclareLocalVariable(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t slot);
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
uint32_t BindUpvalueCell(EngineContext* ctx, uint32_t state, uint32_t local, uint32_t upvalue);
void PrepareParameterFrame(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
uint32_t GrowArray(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
uint32_t CompileStatement(EngineContext* ctx, uint32_t state);
uint32_t EmitInstructionABC(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t arg_a, uint32_t arg_c);

namespace {

// The `'%s' expected` message the '(' path reports.
constexpr uint32_t kParseParamListMsgExpected = 0x2B625u;
// The `chunk has too many syntax levels` diagnostic.
constexpr uint32_t kParseParamListMsgChunkTooDeep = 0x2B633u;
// The name of the implicit `self` parameter.
constexpr uint32_t kParseParamListNameSelf = 0x2B70Au;
// The name of the `...` vararg parameter.
constexpr uint32_t kParseParamListNameVarArg = 0x2B738u;
// The `'<name>' or '...' expected` message.
constexpr uint32_t kParseParamListMsgNameExpected = 0x2B73Cu;
// The message reported when the constant table cannot take another entry.
constexpr uint32_t kParseParamListMsgConstantOverflow = 0x2B755u;
// The cap on the entries grown into the constant table.
constexpr uint32_t kParseParamListMaxConstantCount = 0x3FFFFu;
// The token bits of the codes that can open a statement.
constexpr uint32_t kParseParamListStatementTokenMask = 0x8010007u;

}  // namespace

// Parses a parameter list and binds each parameter name as a local slot: `self` for an implicit
// method, `...` for a vararg list, one local per declared name. Every call that can run guest
// code is a suspend gate -- a rewinding pass restores the frame slots below and replays the one
// call its resume code names. `a` is the lexer state, `b` the parameter descriptor the caller
// fills in, `c` the slot count and `d` the descriptor of the parameter being declared; `b`, `c`
// and `d` also carry the temporaries the flattened driver keeps in them.
void ParseParamList(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 40u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the calls below spill, in the order they run.
  uint32_t const kPrepareParameterCall = 0u;
  uint32_t const kFormatCharCall = 1u;
  uint32_t const kFormatExpectedCall = 2u;
  uint32_t const kSyntaxErrorCall = 3u;
  uint32_t const kRecordStatementCall = 4u;
  uint32_t const kBindSelfCall = 5u;
  uint32_t const kDeclareSelfCall = 6u;
  uint32_t const kRecordParameterCall = 7u;
  uint32_t const kDeclareNameCall = 8u;
  uint32_t const kRecordVarArgCall = 9u;
  uint32_t const kBindVarArgCall = 10u;
  uint32_t const kDeclareVarArgCall = 11u;
  uint32_t const kNameExpectedCall = 12u;
  uint32_t const kRecordCommaCall = 13u;
  uint32_t const kReserveRegistersCall = 14u;
  uint32_t const kFormatCloseCall = 15u;
  uint32_t const kFormatCloseExpectedCall = 16u;
  uint32_t const kSyntaxErrorCloseCall = 17u;
  uint32_t const kRecordListEndCall = 18u;
  uint32_t const kSyntaxLevelsCall = 19u;
  uint32_t const kCompileStatementCall = 20u;
  uint32_t const kRecordBodyCall = 21u;
  uint32_t const kCheckClosingMatchCall = 22u;
  uint32_t const kPatchUpvalueCall = 23u;
  uint32_t const kGrowConstantsCall = 24u;
  uint32_t const kEmitClosureCall = 25u;
  uint32_t const kEmitParameterCall = 26u;
  // The frame saves one word per slot below, so each slot keeps a single name across the whole
  // step even where a later region reuses it: the general temporary, the temporary that holds
  // each call result, the resume code in flight, the item under examination (the token, a name
  // pointer, the block or the constant array), the container the names are read from, the array
  // being written, the 608-byte guest frame the parameters are collected on and a flag.
  uint32_t scratch = 0u;
  uint32_t call_result = 0u;
  uint32_t resume_code = 0u;
  uint32_t item = 0u;
  uint32_t container = 0u;
  uint32_t target = 0u;
  uint32_t scratch_frame = 0u;
  uint32_t flag = 0u;
  uint32_t array = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame and restore the slots in the order the spill path saved them.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, saved_frame);
    c = LoadU32(ctx, saved_frame + 8u);
    d = LoadU32(ctx, saved_frame + 12u);
    scratch = LoadU32(ctx, saved_frame + 16u);
    item = LoadU32(ctx, saved_frame + 20u);
    container = LoadU32(ctx, saved_frame + 24u);
    target = LoadU32(ctx, saved_frame + 28u);
    scratch_frame = LoadU32(ctx, saved_frame + 32u);
    flag = LoadU32(ctx, saved_frame + 36u);
    b = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the 608-byte guest frame the parameter names are collected on.
    scratch_frame = GuestStackSp(ctx) - 608u;
    GuestStackSp(ctx) = scratch_frame;
    scratch = scratch_frame + 32u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPrepareParameterCall) : 1u) {
    // Lay the parameter names out on the guest frame.
    PrepareParameterFrame(ctx, a, scratch);
    resume_code = kPrepareParameterCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Publish the descriptor the caller handed in and flag a '(' that did not open the list.
    StoreU32(ctx, LoadU32(ctx, scratch_frame + 32u) + 60u, d);
    scratch = static_cast<uint32_t>(LoadU32(ctx, a + 16u) != 40u);
  }
  if (scratch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      container = LoadU32(ctx, a + 52u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatCharCall) : 1u) {
      call_result = FormatCharValue(ctx, a, 40u);
      resume_code = kFormatCharCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      scratch = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scratch_frame + 16u, scratch);
      scratch = scratch_frame + 16u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatExpectedCall) : 1u) {
      call_result = FormatPushString(ctx, container, kParseParamListMsgExpected, scratch);
      resume_code = kFormatExpectedCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      scratch = call_result;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSyntaxErrorCall) : 1u) {
      RaiseSyntaxError(ctx, a, scratch);
      resume_code = kSyntaxErrorCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordStatementCall) : 1u) {
    StepRecordResume(ctx, a);
    resume_code = kRecordStatementCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A name list rather than a single name: remember whether it was already bound.
    c = static_cast<uint32_t>(c == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (c != 0u) {
      // The list is non-empty, so no implicit `self` is bound.
      container = LoadU32(ctx, a + 48u);
      target = LoadU32(ctx, container);
      goto parameters_ready;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kBindSelfCall) : 1u) {
    call_result = BindUpvalueCell(ctx, a, kParseParamListNameSelf, 4u);
    resume_code = kBindSelfCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    c = call_result;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDeclareSelfCall) : 1u) {
    DeclareLocalVariable(ctx, a, c, 0u);
    resume_code = kDeclareSelfCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the slot the next parameter will occupy in the pending-name list.
    container = LoadU32(ctx, a + 48u);
    c = LoadU8(ctx, container + 50u) + 1u;
    StoreU8(ctx, container + 50u, c);
    target = LoadU32(ctx, container);
    c = LoadU32(ctx, target + 24u) + LoadU16(ctx, container + 170u + ((c & 255u) << 1u)) * 12u;
    scratch = LoadU32(ctx, container + 24u);
    StoreU32(ctx, c + 4u, scratch);
  }
  parameters_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Clear the stale list flag before the names are scanned.
    StoreU8(ctx, target + 74u, 0u);
    item = LoadU32(ctx, a + 16u);
    flag = static_cast<uint32_t>(item == 41u);
    scratch = 0u;
    c = 0u;
  }
  if (flag != 0u && ResumePhase(ctx) == kResumeRunning) {
    goto parameters_done;
  }
  for (;;) {
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = item - 279u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (scratch == 0u) {
        goto bind_self_param;
      }
      if (scratch != 6u) {
        goto name_expected;
      }
      item = LoadU32(ctx, a + 24u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordParameterCall) : 1u) {
      StepRecordResume(ctx, a);
      resume_code = kRecordParameterCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDeclareNameCall) : 1u) {
      DeclareLocalVariable(ctx, a, item, c);
      resume_code = kDeclareNameCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      c = c + 1u;
      goto parameter_scanned;
    }
    bind_self_param:;
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordVarArgCall) : 1u) {
      StepRecordResume(ctx, a);
      resume_code = kRecordVarArgCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kBindVarArgCall) : 1u) {
      call_result = BindUpvalueCell(ctx, a, kParseParamListNameVarArg, 3u);
      resume_code = kBindVarArgCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      scratch = call_result;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDeclareVarArgCall) : 1u) {
      DeclareLocalVariable(ctx, a, scratch, c);
      resume_code = kDeclareVarArgCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, target + 74u, 7u);
      c = c + 1u;
      scratch = 1u;
      goto parameters_done;
    }
    name_expected:;
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNameExpectedCall) : 1u) {
      RaiseSyntaxError(ctx, a, kParseParamListMsgNameExpected);
      resume_code = kNameExpectedCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    parameter_scanned:;
    if (ResumePhase(ctx) == kResumeRunning) {
      item = LoadU8(ctx, target + 74u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (item != 0u) {
        break;
      }
      scratch = static_cast<uint32_t>(LoadU32(ctx, a + 16u) != 44u);
      if (scratch != 0u) {
        break;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordCommaCall) : 1u) {
      StepRecordResume(ctx, a);
      resume_code = kRecordCommaCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      item = LoadU32(ctx, a + 16u);
      continue;
    }
    break;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = item & 1u;
  }
  parameters_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Publish the collected count and fill the constant-table slots the names reserve.
    item = LoadU32(ctx, a + 48u);
    flag = LoadU8(ctx, item + 50u) + c;
    StoreU8(ctx, item + 50u, flag);
    if (c != 0u) {
      flag = flag & 255u;
      uint32_t const entry_value = LoadU32(ctx, item + 24u);
      array = LoadU32(ctx, LoadU32(ctx, item) + 24u);
      do {
        StoreU32(ctx, array + LoadU16(ctx, item + 172u + ((flag - c) << 1u)) * 12u + 4u, entry_value);
        c = c - 1u;
      } while (c != 0u);
    }
    c = LoadU8(ctx, container + 50u);
    scratch = c - scratch;
    StoreU8(ctx, target + 73u, scratch);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReserveRegistersCall) : 1u) {
    ReserveRegisters(ctx, container, c);
    resume_code = kReserveRegistersCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    c = static_cast<uint32_t>(LoadU32(ctx, a + 16u) != 41u);
  }
  if (c != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      c = LoadU32(ctx, a + 52u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatCloseCall) : 1u) {
      call_result = FormatCharValue(ctx, a, 41u);
      resume_code = kFormatCloseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      scratch = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scratch_frame, scratch);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatCloseExpectedCall) : 1u) {
      call_result = FormatPushString(ctx, c, kParseParamListMsgExpected, scratch_frame);
      resume_code = kFormatCloseExpectedCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      c = call_result;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSyntaxErrorCloseCall) : 1u) {
      RaiseSyntaxError(ctx, a, c);
      resume_code = kSyntaxErrorCloseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordListEndCall) : 1u) {
    StepRecordResume(ctx, a);
    resume_code = kRecordListEndCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Count one syntax level for this list and flag a level count that is already too deep.
    c = LoadU32(ctx, a + 52u);
    scratch = c;
    c = LoadU16(ctx, scratch + 52u) + 1u;
    StoreU16(ctx, scratch + 52u, c);
    c = static_cast<uint32_t>((c & 65535u) >= 201u);
  }
  if (c != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSyntaxLevelsCall) : 1u) {
      LexError(ctx, a, kParseParamListMsgChunkTooDeep, 0u);
      resume_code = kSyntaxLevelsCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  for (;;) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Body statements are compiled to fill the parameter list's closing match.
      c = LoadU32(ctx, a + 16u) - 260u;
      scratch = static_cast<uint32_t>(c <= 27u);
      uint32_t const bit_set = static_cast<uint32_t>(((1u << (c & 31u)) & kParseParamListStatementTokenMask) != 0u);
      c = static_cast<uint32_t>((bit_set != 0u ? scratch : 0u) == 0u);
    }
    if (c != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCompileStatementCall) : 1u) {
        call_result = CompileStatement(ctx, a);
        resume_code = kCompileStatementCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        c = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        scratch = static_cast<uint32_t>(LoadU32(ctx, a + 16u) == 59u);
      }
      if (scratch != 0u || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordBodyCall) : 1u) {
          StepRecordResume(ctx, a);
          resume_code = kRecordBodyCall;
          if (ResumePhase(ctx) == kResumeSuspended) {
            goto suspend_point;
          }
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        container = LoadU32(ctx, a + 48u);
        scratch = LoadU8(ctx, container + 50u);
        StoreU32(ctx, container + 36u, scratch);
        c = static_cast<uint32_t>(c == 0u);
        if (c != 0u) {
          continue;
        }
      }
    }
    break;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Close the list: hand the level count back and publish the parameter count.
    c = LoadU32(ctx, a + 52u);
    scratch = LoadU16(ctx, c + 52u) - 1u;
    StoreU16(ctx, c + 52u, scratch);
    c = LoadU32(ctx, scratch_frame + 32u);
    scratch = LoadU32(ctx, a + 4u);
    StoreU32(ctx, c + 64u, scratch);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCheckClosingMatchCall) : 1u) {
    CheckClosingMatch(ctx, a, 262u, 265u, d);
    resume_code = kCheckClosingMatchCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPatchUpvalueCall) : 1u) {
    PatchUpvalueJumpTarget(ctx, a);
    resume_code = kPatchUpvalueCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Type the parameter table: a table that cannot take another entry has to grow.
    container = LoadU32(ctx, a + 48u);
    scratch = LoadU32(ctx, container + 44u);
    d = LoadU32(ctx, container);
    c = LoadU32(ctx, d + 52u);
    scratch = static_cast<uint32_t>(static_cast<int32_t>(scratch) < static_cast<int32_t>(c));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scratch != 0u) {
      goto constant_table_done;
    }
    item = LoadU32(ctx, d + 16u);
    target = d + 52u;
    scratch = LoadU32(ctx, a + 52u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kGrowConstantsCall) : 1u) {
    call_result = GrowArray(ctx, scratch, item, target, 4u, kParseParamListMaxConstantCount,
                            kParseParamListMsgConstantOverflow);
    resume_code = kGrowConstantsCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    item = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Store the grown table and clear the entries the old one left behind.
    StoreU32(ctx, d + 16u, item);
    target = LoadU32(ctx, d + 52u);
    if (static_cast<int32_t>(target) <= static_cast<int32_t>(c)) {
      goto constant_table_done;
    }
    StoreU32(ctx, item + (c << 2u), 0u);
    c = c + 1u;
    if (target != c) {
      do {
        StoreU32(ctx, LoadU32(ctx, d + 16u) + (c << 2u), 0u);
        c = c + 1u;
      } while (target != c);
    }
  }
  constant_table_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Append the entry for this parameter to the list the caller is filling in.
    target = LoadU32(ctx, d + 16u);
    item = LoadU32(ctx, scratch_frame + 32u);
    scratch = LoadU32(ctx, container + 44u);
    c = scratch + 1u;
    StoreU32(ctx, container + 44u, c);
    StoreU32(ctx, target + (scratch << 2u), item);
    target = LoadU32(ctx, scratch_frame + 32u);
    if ((LoadU8(ctx, target + 5u) & 3u) != 0u && (LoadU8(ctx, d + 5u) & 4u) != 0u) {
      StoreListEntry(ctx, LoadU32(ctx, a + 52u), d, target);
      c = LoadU32(ctx, container + 44u);
    }
    c = c - 1u;
    a = 0u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEmitClosureCall) : 1u) {
    call_result = EmitInstructionABC(ctx, container, 36u, 0u, c);
    resume_code = kEmitClosureCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    c = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Hand the closure and the parameter count back through the descriptor.
    StoreU64(ctx, b + 16u, 18446744073709551615ull);
    StoreU32(ctx, b + 8u, c);
    StoreU32(ctx, b, 11u);
    b = LoadU8(ctx, LoadU32(ctx, scratch_frame + 32u) + 72u);
  }
  if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
    for (;;) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // One register per parameter slot: load the argument and pass it on.
        b = scratch_frame + 32u + (a << 1u);
        c = static_cast<uint32_t>(LoadU8(ctx, b + 51u) != 6u) << 2u;
        b = LoadU8(ctx, b + 52u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEmitParameterCall) : 1u) {
        (void)EmitOpInstruction(ctx, container, c, 0u, b, 0u);
        resume_code = kEmitParameterCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        a = a + 1u;
        b = static_cast<uint32_t>(a < LoadU8(ctx, LoadU32(ctx, scratch_frame + 32u) + 72u));
        if (b != 0u) {
          continue;
        }
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch_frame + 608u;
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: store the code of the call in flight, step past it, then push the 40-byte frame
    // a rewinding pass reads back -- one word per slot, in the order they were saved.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, a);
    StoreU32(ctx, saved_frame + 4u, b);
    StoreU32(ctx, saved_frame + 8u, c);
    StoreU32(ctx, saved_frame + 12u, d);
    StoreU32(ctx, saved_frame + 16u, scratch);
    StoreU32(ctx, saved_frame + 20u, item);
    StoreU32(ctx, saved_frame + 24u, container);
    StoreU32(ctx, saved_frame + 28u, target);
    StoreU32(ctx, saved_frame + 32u, scratch_frame);
    StoreU32(ctx, saved_frame + 36u, flag);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
