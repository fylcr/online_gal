// resolve_table_field.cpp — writable value record for a table key.
//
// ResolveTableField is the assignment side of the table's keyed access: where
// LookupTableField answers with an existing record or the shared nil record,
// this entry always hands back a record that can be written. It first re-runs
// the key lookup, and only when that lookup missed does it judge the key itself
// — a key whose type tag is nil raises the guest's own "table index is nil"
// text, a numeric key whose f64 equals no value raises "table index is NaN", and
// every other tag goes straight to creating the key. The table's flag byte is
// cleared before either diagnostic is raised, so a caller that traps and
// recovers still sees a table whose cached shape was invalidated.
//
// The body also carries the engine's asyncify state machine. Phase rewinding
// restores the parameters and the working value out of the event return frame
// before anything else, because the steps below that are guarded on the phase
// would otherwise be skipped; phase suspended means a call inside this frame
// already unwound, and the frame then spills the code naming that call and
// answers zero. That code is what a rewinding frame reads back, so each guarded
// step is replayed for the one call that was actually in flight and no other.
// Judgment order is the engine's own throughout: the running select is taken
// before the rewinding bit is merged into it, the nil test runs before the NaN
// test, and every resumable call is followed by its own suspension check.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in script/lookup_table_field.cpp.
uint32_t LookupTableField(EngineContext* ctx, uint32_t table, uint32_t key);
// Forward declaration — defined in script/raise_script_error.cpp.
// PROVISIONAL: the generated f313 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);

namespace {

// The code a suspending frame spills names the call it was inside, so a resumed
// frame replays exactly that call.
constexpr uint32_t kNilKeyCall = 0u;
constexpr uint32_t kNaNKeyCall = 1u;
constexpr uint32_t kCreateKeyCall = 2u;

// Both diagnostics sit in the guest's string pool beside the shared nil record,
// so they are named off that anchor instead of as raw pool addresses.
constexpr uint32_t kNilIndexTextAddr = kNilValueAddr + 2155u;
constexpr uint32_t kNaNIndexTextAddr = kNilValueAddr + 2174u;

}  // namespace

uint32_t ResolveTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  uint32_t suspended_call = 0u;
  uint32_t lookup_skipped = 0u;
  uint32_t spill_code = 0u;
  double key_number = 0.0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Undo the parameter spill. The event return frame is reached through the
    // frame pointer the slot holds, and the frame keeps the three parameters
    // first and the working value last, in the order they are read back here.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    result = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, result);
    table = LoadU32(ctx, result + 4u);
    key = LoadU32(ctx, result + 8u);
    result = LoadU32(ctx, result + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The spilled call code sits one word below the saved parameters.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    suspended_call = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A frame that is resuming never re-runs the lookup, so the placeholder is
  // what the select below sees; a running frame takes the lookup's miss test.
  uint32_t dispatch_live = resume_code;
  uint32_t lookup_miss;
  if (ResumePhase(ctx)) {
    lookup_miss = lookup_skipped;
  } else {
    lookup_miss = LookupTableField(ctx, table, key);
    result = lookup_miss;
    StoreU8(ctx, table + 6u, 0u);
    lookup_miss = static_cast<uint32_t>(result == kNilValueAddr);
  }
  dispatch_live = ResumePhase(ctx) ? dispatch_live : lookup_miss;
  dispatch_live |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (dispatch_live) {
    if (!(ResumePhase(ctx))) {
      // A running frame judges the key's own type tag.
      result = LoadU32(ctx, key + 8u);
    }
    if (!(ResumePhase(ctx))) {
      switch (result) {
        case 0u: break;
        case 1u: goto create_key;
        case 2u: goto create_key;
        case 3u: goto check_nan_key;
        default: goto create_key;
      }
    }
    {
      uint32_t const taken = static_cast<uint32_t>(!suspended_call);
      if (ResumePhase(ctx) ? taken : 1u) {
        RaiseScriptError(ctx, state, kNilIndexTextAddr, 0u);
        spill_code = kNilKeyCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_frame;
        }
      }
    }
    if (!(ResumePhase(ctx))) {
      goto create_key;
    }
  check_nan_key:
    {
      if (!(ResumePhase(ctx))) {
        // A numeric key is still unusable when it is NaN, and the engine's only
        // test for that is the read value against its own copy.
        double const key_read = LoadF64(ctx, key);
        key_number = key_read;
        result = static_cast<uint32_t>(key_read == key_number);
        if (result) {
          goto create_key;
        }
      }
      uint32_t const taken = static_cast<uint32_t>(suspended_call == kNaNKeyCall);
      if (ResumePhase(ctx) ? taken : 1u) {
        RaiseScriptError(ctx, state, kNaNIndexTextAddr, 0u);
        spill_code = kNaNKeyCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_frame;
        }
      }
    }
  create_key:
    {
      uint32_t const taken = static_cast<uint32_t>(suspended_call == kCreateKeyCall);
      if (ResumePhase(ctx) ? taken : 1u) {
        resume_code = FinishTableIndex(ctx, state, table, key);
        spill_code = kCreateKeyCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_frame;
        }
        result = resume_code;
      }
    }
  }
  if (!(ResumePhase(ctx))) {
    LeaveFrame();
    return result;
  }
  TrapUnreachable();
suspend_frame:
  // Hand the rewinder the code naming the call to resume, then the frame this
  // one's own parameter spill reads back. The code word goes below the frame
  // pointer, which then advances past it to make room for the parameters.
  resume_code = spill_code;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  resume_code = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, resume_code, state);
  StoreU32(ctx, resume_code + 4u, table);
  StoreU32(ctx, resume_code + 8u, key);
  StoreU32(ctx, resume_code + 12u, result);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
