#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the read-only lookup and the guest diagnostic raiser.
uint32_t LookupTableField(EngineContext* ctx, uint32_t table, uint32_t key);
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);

namespace {

// The code a suspending frame spills names the call it was inside, so a resumed
// frame replays exactly that call.
constexpr uint32_t kNilKeyCall = 0u;
constexpr uint32_t kNaNKeyCall = 1u;
constexpr uint32_t kCreateKeyCall = 2u;

// Both diagnostics sit in the guest's string pool beside the shared nil record.
constexpr uint32_t kNilIndexTextAddr = kNilValueAddr + 2155u;
constexpr uint32_t kNaNIndexTextAddr = kNilValueAddr + 2174u;

}  // namespace

// The writing side of the table's keyed access: the lookup runs first, and only
// when it missed is the key judged -- a nil key and a NaN number raise the
// guest's own diagnostic, every other key is created and answered with the
// record it now names.
uint32_t ResolveTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  uint32_t suspended_call = 0u;
  uint32_t lookup_skipped = 0u;
  uint32_t spill_code = 0u;
  double key_number = 0.0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Undo the parameter spill: the frame keeps the three parameters first and
    // the working value last, in the order they are read back here.
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
  uint32_t dispatch_live = resume_code;
  uint32_t lookup_miss;
  if (ResumePhase(ctx)) {
    lookup_miss = lookup_skipped;
  } else {
    lookup_miss = LookupTableField(ctx, table, key);
    result = lookup_miss;
    // The table's cached shape is invalidated before either diagnostic can
    // raise, so a caller that recovers still sees it cleared.
    StoreU8(ctx, table + 6u, 0u);
    lookup_miss = static_cast<uint32_t>(result == kNilValueAddr);
  }
  dispatch_live = ResumePhase(ctx) ? dispatch_live : lookup_miss;
  dispatch_live |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (dispatch_live) {
    if (!(ResumePhase(ctx))) {
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
        // A numeric key is unusable when it is NaN, and the read value against
        // its own copy is the only test for that.
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
  // one's own parameter spill reads back.
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
