#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the registered callees this step drives.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ApplyEventRecordTable(EngineContext* ctx, uint32_t a);
void FormatLuaTagFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

// Message ids, the pending-throw word and the frame sizes this step uses.
constexpr uint32_t kPersistMsgAutosaveDat = 0xAFAF2u;       // autosave.dat
constexpr uint32_t kPersistMsgWaitOut = 0xB4ACBu;           // [Lua] onClickWaitOut %s: %s
constexpr uint32_t kPersistMsgWaitIn = 0xB4AE7u;            // [Lua] onClickWaitIn %s: %s
constexpr uint32_t kPersistMsgAssertCondition = 0xB5E31u;   // px != 0
constexpr uint32_t kPersistMsgAssertFile = 0xB5E39u;        // boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kPersistMsgAssertName = 0xB5E71u;        // operator->
constexpr uint32_t kPersistThrowWord = 0xD015Cu;
constexpr uint32_t kPersistRecordFlags = 0x80000010u;
constexpr uint32_t kPersistScratchBase = 0xFFFFFFC0u;
constexpr uint32_t kPersistIndexMax = 0xFFFFFFFFu;
constexpr uint32_t kPersistSaveFrameBytes = 24u;
constexpr uint32_t kPersistResumeWordBytes = 4u;
constexpr uint32_t kPersistStackBytes = 80u;
constexpr uint32_t kPersistModeOffset = 2876u;

}  // namespace

// Saves the session: the autosave write plus the onClickWaitIn and
// onClickWaitOut Lua callbacks, in the order the step's mode selects.  Each
// host call can suspend the step, so its entry gate replays that call from
// the resume code the suspended pass stored.  `session` and `state` also
// serve as scratch registers, exactly as the step's own frame does.
void PersistSessionState(EngineContext* ctx, uint32_t session, uint32_t state) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t latch = 0u;
  uint32_t flag = 0u;
  uint32_t object = 0u;
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  uint32_t value = 0u;
  uint32_t operand1 = 0u;
  uint32_t operand2 = 0u;
  uint64_t zero64 = 0;

  // Resume codes -- the call each gate replays on a rewinding pass.
  constexpr uint32_t kAssertHead = 0u;
  constexpr uint32_t kAssertGate = 1u;
  constexpr uint32_t kCallWaitOut = 2u;
  constexpr uint32_t kAssertPath = 3u;
  constexpr uint32_t kDispatchWaitOut = 4u;
  constexpr uint32_t kAssertResult = 5u;
  constexpr uint32_t kFormatWaitOut = 6u;
  constexpr uint32_t kApplyEvents = 7u;
  constexpr uint32_t kCallWaitIn = 8u;
  constexpr uint32_t kAssertRetry = 9u;
  constexpr uint32_t kDispatchWaitIn = 10u;
  constexpr uint32_t kAssertRelease = 11u;
  constexpr uint32_t kFormatWaitIn = 12u;
  constexpr uint32_t kAssertSave = 13u;
  constexpr uint32_t kAllocName = 14u;
  constexpr uint32_t kAssertRecord = 15u;
  constexpr uint32_t kFormatRecord = 16u;
  constexpr uint32_t kAssertFormat = 17u;
  constexpr uint32_t kAssertEvent = 18u;
  constexpr uint32_t kFormatEvent = 19u;
  constexpr uint32_t kPanicHandoff = 20u;

  // The two pops that a rewinding pass performs: the first rewinds the saved
  // frame pointer past the register block and restores the registers, the
  // second takes the resume code off the same frame.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kPersistSaveFrameBytes);
    result = LoadU32(ctx, ReturnFrameSp(ctx));
    session = LoadU32(ctx, result);
    frame = LoadU32(ctx, result + 8u);
    latch = LoadU32(ctx, result + 12u);
    flag = LoadU32(ctx, result + 16u);
    object = LoadU32(ctx, result + 20u);
    state = LoadU32(ctx, result + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kPersistResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  // Carve the step's work area out of the guest stack and read the mode word
  // that tells whether there is anything left to save.
  if (ResumePhase(ctx) == kResumeRunning) {
    frame = GuestStackSp(ctx) - kPersistStackBytes;
    GuestStackSp(ctx) = frame;
    latch = LoadU32(ctx, session + kPersistModeOffset);
    flag = static_cast<uint32_t>(latch == 13u);
  }
  // Mode 13 means the session was already saved.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag) { goto dispatch_done; }
    StoreU32(ctx, session + kPersistModeOffset, state);
    StoreU32(ctx, session + 2880u, latch);
    flag = static_cast<uint32_t>(state != 1u);
    object = LoadU32(ctx, session + 8u);
  }
  // Decide between the destroy path (the object is gone) and the wait path.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag) { goto path_gate; }
    flag = static_cast<uint32_t>(LoadU8(ctx, session + 34u) == 0u);
    if (flag) { goto path_gate; }
    if (object) { goto destroy_await; }
    StoreU32(ctx, kPersistThrowWord, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertHead) : 1u) {
    HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
    value = kAssertHead;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    session = LoadU32(ctx, kPersistThrowWord);
    StoreU32(ctx, kPersistThrowWord, 0u);
    session = static_cast<uint32_t>(session != 1u);
    if (session) { goto unreachable_trap; }
    goto throw_unwind;
  }

  // The wait path: a null object jumps straight to the tail, otherwise the
  // waiting callback is taken from the object's own vtable slot 28.
  path_gate:;
  flag = ResumePhase(ctx) ? flag : static_cast<uint32_t>(object == 0u);
  if (flag || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertGate) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertGate;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = LoadU32(ctx, LoadU32(ctx, object) + 28u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCallWaitOut) : 1u) {
    CallTableT0(ctx, flag, object, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    value = kCallWaitOut;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Dispatch the mode: the switch picks the sub-step and every unlisted mode
  // is a no-op for this pass.
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (state) {
      case 0: goto mode_zero;
      case 1: goto destroy_path;
      case 3: goto autosave_path;
      case 5: goto event_path;
      case 6: goto record_filter_path;
      case 14: goto null_object_path;
      default: goto dispatch_done;
    }
  mode_zero:;
    state = static_cast<uint32_t>(latch != 1u);
  }
  // Mode 0: the availability byte at +14391 decides whether the save can
  // start; a negative length means the path field itself holds the value.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (state) { goto mode_zero_tail; }
    state = LoadS8(ctx, session + 14391u);
    latch = static_cast<uint32_t>(static_cast<int32_t>(state) <= static_cast<int32_t>(kPersistIndexMax));
    value = latch ? LoadU32(ctx, session + 14384u) : (state & 255u);
    state = static_cast<uint32_t>(value == 0u);
    if (state) { goto mode_zero_tail; }
    state = LoadU32(ctx, session + 136u);
    latch = static_cast<uint32_t>(state == 0u);
  }
  if (latch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertPath) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertPath;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  // Build the record the dispatch call takes: the flag word, the state and
  // the scratch pointer live at +68, +64 of the work area.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, frame + 68u, zero64);
    StoreU32(ctx, kPersistThrowWord, 0u);
    StoreU32(ctx, frame + 64u, (frame - kPersistScratchBase) | 4u);
    latch = state;
    flag = frame - kPersistScratchBase;
    state = session + 14380u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDispatchWaitOut) : 1u) {
    result = HostDispatchInvoke(ctx, 2595u, latch, state, flag);
    value = kDispatchWaitOut;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    object = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    latch = LoadU32(ctx, kPersistThrowWord);
    StoreU32(ctx, kPersistThrowWord, 0u);
    if (latch == 1u) { goto throw_exit; }
    value = frame - kPersistScratchBase;
    operand1 = LoadU32(ctx, frame + 68u);
    flag = operand1;
    ReleaseNodeTree(ctx, value, operand1);
    if (object == 0u) { goto mode_zero_tail; }
    object = LoadU32(ctx, session + 136u);
    flag = static_cast<uint32_t>(object == 0u);
    value = LoadS8(ctx, state + 11u);
    if (static_cast<uint32_t>(static_cast<int32_t>(value) <= static_cast<int32_t>(kPersistIndexMax))) {
      value = LoadU32(ctx, state);
      latch = value;
    } else {
      latch = state;
    }
  }
  if (flag || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertResult) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertResult;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  // The wait out call takes the string the object owns: the length byte at
  // +15 selects the inline buffer at +4 or the pointer field.
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = object + 4u;
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, object + 15u)) <= static_cast<int32_t>(kPersistIndexMax))) {
      flag = LoadU32(ctx, object + 4u);
    }
    StoreU32(ctx, frame + 20u, flag);
    StoreU32(ctx, frame + 16u, latch);
    latch = frame + 16u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatWaitOut) : 1u) {
    FormatLuaTagFilterLine(ctx, session, 1u, kPersistMsgWaitOut, latch);
    value = kFormatWaitOut;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // The wait out callback returns the object into an empty state, so its
  // string is released here.
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadS8(ctx, session + 14391u);
    latch = static_cast<uint32_t>(static_cast<int32_t>(value) <= static_cast<int32_t>(kPersistIndexMax));
    if (latch) {
      StoreU8(ctx, LoadU32(ctx, session + 14380u), 0u);
      state = session + 14384u;
      StoreU32(ctx, state, 0u);
      goto mode_zero_tail;
    }
    StoreU8(ctx, state + 11u, 0u);
    StoreU8(ctx, state, 0u);
  }

  mode_zero_tail:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, session + 2768u, zero64);
    state = session + 2792u;
    StoreU8(ctx, state, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kApplyEvents) : 1u) {
    ApplyEventRecordTable(ctx, session);
    value = kApplyEvents;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto dispatch_done; }

  // Mode 2: the waiting object is destroyed with the second vtable call.
  destroy_await:;
  if (ResumePhase(ctx) == kResumeRunning) {
    state = LoadU32(ctx, LoadU32(ctx, object) + 28u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCallWaitIn) : 1u) {
    CallTableT0(ctx, state, object, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    value = kCallWaitIn;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }

  // Mode 1: release the object the session still holds.  The length byte at
  // +14379 selects between the inline path and the owned pointer.
  destroy_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    state = LoadS8(ctx, session + 14379u);
    if (static_cast<uint32_t>(static_cast<int32_t>(state) <= static_cast<int32_t>(kPersistIndexMax))) {
      value = LoadU32(ctx, session + 14372u);
      state = static_cast<uint32_t>(value == 0u);
    } else {
      state = static_cast<uint32_t>((state & 255u) == 0u);
    }
    if (state) { goto autosave_path; }
    state = LoadU32(ctx, session + 136u);
    latch = static_cast<uint32_t>(state == 0u);
  }
  if (latch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertRetry) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertRetry;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  // The wait in dispatch takes the string the session owns.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, frame + 68u, zero64);
    StoreU32(ctx, kPersistThrowWord, 0u);
    StoreU32(ctx, frame + 64u, (frame - kPersistScratchBase) | 4u);
    latch = state;
    flag = frame - kPersistScratchBase;
    state = session + 14368u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDispatchWaitIn) : 1u) {
    result = HostDispatchInvoke(ctx, 2595u, latch, state, flag);
    value = kDispatchWaitIn;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    object = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    latch = LoadU32(ctx, kPersistThrowWord);
    StoreU32(ctx, kPersistThrowWord, 0u);
    if (latch == 1u) { goto throw_exit; }
    value = frame - kPersistScratchBase;
    operand1 = LoadU32(ctx, frame + 68u);
    flag = operand1;
    ReleaseNodeTree(ctx, value, operand1);
    if (object == 0u) { goto autosave_path; }
    object = LoadU32(ctx, session + 136u);
    flag = static_cast<uint32_t>(object == 0u);
    value = LoadS8(ctx, state + 11u);
    if (static_cast<uint32_t>(static_cast<int32_t>(value) <= static_cast<int32_t>(kPersistIndexMax))) {
      value = LoadU32(ctx, state);
      latch = value;
    } else {
      latch = state;
    }
  }
  if (flag || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertRelease) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertRelease;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  // The wait in call takes the same string, this time by value.
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = object + 4u;
    operand1 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, object + 15u)) <= static_cast<int32_t>(kPersistIndexMax));
    if (operand1) {
      operand1 = LoadU32(ctx, object + 4u);
    } else {
      operand1 = flag;
    }
    StoreU32(ctx, frame + 4u, operand1);
    StoreU32(ctx, frame, latch);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatWaitIn) : 1u) {
    FormatLuaTagFilterLine(ctx, session, 1u, kPersistMsgWaitIn, frame);
    value = kFormatWaitIn;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadS8(ctx, session + 14379u);
    if (static_cast<uint32_t>(static_cast<int32_t>(value) <= static_cast<int32_t>(kPersistIndexMax))) {
      StoreU8(ctx, LoadU32(ctx, session + 14368u), 0u);
      state = session + 14372u;
      StoreU32(ctx, state, 0u);
      goto autosave_path;
    }
    StoreU8(ctx, state + 11u, 0u);
    StoreU8(ctx, state, 0u);
  }

  // Mode 3: write the autosave record.  The record is skipped when the
  // session is already live or has a pending event table.
  autosave_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU8(ctx, session + 48u)) {
      if (LoadU8(ctx, session + 49u)) { goto dispatch_done; }
      if (LoadU8(ctx, session + 50u)) { goto dispatch_done; }
    }
    if (LoadU32(ctx, session + 13212u)) { goto dispatch_done; }
    object = LoadU32(ctx, session + 128u);
    state = static_cast<uint32_t>(object == 0u);
  }
  if (state || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertSave) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertSave;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  // The record name is "autosave.dat" (12 bytes) or empty, and `state` is
  // the flag that says whether the record was live.
  if (ResumePhase(ctx) == kResumeRunning) {
    operand2 = LoadU32(ctx, session + 2956u);
    state = static_cast<uint32_t>(operand2 == 2u);
    session = state ? 12u : 0u;
  }
  if (state || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAllocName) : 1u) {
      result = AllocateStringBuffer(ctx, 16u);
      value = kAllocName;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      state = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, frame + 56u, kPersistRecordFlags);
      StoreU32(ctx, frame + 48u, state);
      StoreU32(ctx, frame + 52u, session);
      CopyBytes(ctx, state, kPersistMsgAutosaveDat, session);
      goto autosave_result;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 59u, session);
    state = frame + 48u;
  }

  autosave_result:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, session + state, 0u);
    StoreU32(ctx, kPersistThrowWord, 0u);
    session = frame + 48u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertRecord) : 1u) {
    HostFailAssertion(ctx, 2600u, object, session, 0u, 0u);
    value = kAssertRecord;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // If the record was never live, drop the string it owns; otherwise fall
  // through to the event tables.
  if (ResumePhase(ctx) == kResumeRunning) {
    session = LoadU32(ctx, kPersistThrowWord);
    StoreU32(ctx, kPersistThrowWord, 0u);
    session = static_cast<uint32_t>(session != 1u);
    if (session) {
      value = LoadS8(ctx, frame + 59u);
      session = static_cast<uint32_t>(static_cast<int32_t>(value) > static_cast<int32_t>(kPersistIndexMax));
      if (session) { goto dispatch_done; }
      session = LoadU32(ctx, frame + 48u);
      DeallocateRawBlock(ctx, session);
      goto dispatch_done;
    }
    session = HostRet0(ctx);
    HostTempRet0(ctx);
    value = LoadS8(ctx, frame + 59u);
    state = static_cast<uint32_t>(static_cast<int32_t>(value) > static_cast<int32_t>(kPersistIndexMax));
    if (state) { goto catch_cleanup; }
    value = LoadU32(ctx, frame + 48u);
    DeallocateRawBlock(ctx, value);
    HostOsRelease(ctx, session);
    TrapUnreachable();
  }

  // Mode 6: format the event filter line into the session's own buffer.
  record_filter_path:;
  state = ResumePhase(ctx) ? state : session + 14116u;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatRecord) : 1u) {
    result = FormatLuaEventFilterLine(ctx, session, state);
    value = kFormatRecord;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    session = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto dispatch_done; }

  // Mode 14: the session no longer owns an event buffer.
  null_object_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    session = LoadU32(ctx, session + 128u);
    state = static_cast<uint32_t>(session == 0u);
  }
  if (state || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPersistThrowWord, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertFormat) : 1u) {
      HostFailAssertion(ctx, 857u, kPersistMsgAssertCondition, kPersistMsgAssertFile, 734u, kPersistMsgAssertName);
      value = kAssertFormat;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      session = LoadU32(ctx, kPersistThrowWord);
      StoreU32(ctx, kPersistThrowWord, 0u);
      session = static_cast<uint32_t>(session != 1u);
      if (session) { goto unreachable_trap; }
      goto throw_unwind;
    }
  }
  // Apply the event table the session holds, then release it.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 32u, 0u);
    StoreU8(ctx, frame + 43u, 0u);
    StoreU32(ctx, kPersistThrowWord, 0u);
    state = frame + 32u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAssertEvent) : 1u) {
    HostFailAssertion(ctx, 2600u, session, state, 0u, 1u);
    value = kAssertEvent;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    session = LoadU32(ctx, kPersistThrowWord);
    StoreU32(ctx, kPersistThrowWord, 0u);
    session = static_cast<uint32_t>(session != 1u);
    if (session) {
      value = LoadS8(ctx, frame + 43u);
      session = static_cast<uint32_t>(static_cast<int32_t>(value) > static_cast<int32_t>(kPersistIndexMax));
      if (session) { goto dispatch_done; }
      session = LoadU32(ctx, frame + 32u);
      DeallocateRawBlock(ctx, session);
      goto dispatch_done;
    }
    session = HostRet0(ctx);
    HostTempRet0(ctx);
    value = LoadS8(ctx, frame + 43u);
    state = static_cast<uint32_t>(static_cast<int32_t>(value) > static_cast<int32_t>(kPersistIndexMax));
    if (state) { goto catch_cleanup; }
    state = LoadU32(ctx, frame + 32u);
    DeallocateRawBlock(ctx, state);
  }

  // Unwinding path: release the caught value and stop the pass.
  catch_cleanup:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, session);
    TrapUnreachable();
  }

  // Mode 5: format the event filter line into a freshly built buffer.
  event_path:;
  state = ResumePhase(ctx) ? state : session + 14028u;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatEvent) : 1u) {
    result = FormatLuaEventFilterLine(ctx, session, state);
    value = kFormatEvent;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    session = result;
  }

  // Every mode that has nothing left to do unwinds the work area and leaves.
  dispatch_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = frame + kPersistStackBytes;
    goto exit_frame;
  }
  unreachable_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // The step threw: hand the caught value to the registered handler.
  throw_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    session = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPanicHandoff) : 1u) {
    Panic(ctx, session);
    value = kPanicHandoff;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }

  // The dispatch call threw: release the node tree the work area holds.
  throw_exit:;
  if (ResumePhase(ctx) == kResumeRunning) {
    session = HostRet0(ctx);
    HostTempRet0(ctx);
    value = frame - kPersistScratchBase;
    operand1 = LoadU32(ctx, frame + 68u);
    ReleaseNodeTree(ctx, value, operand1);
    HostOsRelease(ctx, session);
    TrapUnreachable();
  }
  goto exit_frame;

  // Park the step: store the resume code and the live registers on the saved
  // frame, then return to the driver.  The stores must keep this order.
  suspend_point:;
  result = value;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), result);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kPersistResumeWordBytes);
  result = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, result, session);
  StoreU32(ctx, result + 4u, state);
  StoreU32(ctx, result + 8u, frame);
  StoreU32(ctx, result + 12u, latch);
  StoreU32(ctx, result + 16u, flag);
  StoreU32(ctx, result + 20u, object);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kPersistSaveFrameBytes);

  exit_frame:;
  LeaveFrame();
}
}  // namespace artc
