#include "util/engine_context.h"

namespace artc {

// Defined in other products.
uint32_t ResolveTableField(EngineContext* ctx, uint32_t state, uint32_t object, uint32_t key);
void GrowValueStack(EngineContext* ctx, uint32_t state, uint32_t bytes);
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t state, uint32_t object, uint32_t kind);
void CallValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t argument);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t state, uint32_t object, uint32_t key);
void RaiseTypeError(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t message);

namespace {

constexpr uint32_t kIndexTableValueIndexMessage = 0x2BEDFu;  // index
constexpr uint32_t kIndexTableValueLoopMessage = 0x2BEF6u;  // loop in settable
constexpr uint32_t kIndexTableValueStackHeadroom = 0xFFFFFFC0u;  // -64, the headroom kept above the value stack
constexpr uint32_t kIndexTableValueSaveFrameBytes = 36u;  // state, table, key, result, object, metamethod, field, scratch, walk count
constexpr uint32_t kIndexTableValueResumeWordBytes = 4u;
constexpr uint32_t kIndexTableValueResolveCall = 0u;  // the field resolve was in flight
constexpr uint32_t kIndexTableValueTypeErrorCall = 1u;  // the type error raise was in flight
constexpr uint32_t kIndexTableValueGrowCall = 2u;  // the value stack growth was in flight
constexpr uint32_t kIndexTableValueInvokeCall = 3u;  // the metamethod invocation was in flight
constexpr uint32_t kIndexTableValueLoopErrorCall = 4u;  // the setter loop error was in flight

}  // namespace

// Answers a table index: looks the key up on the object, falls back to the
// object's metamethods when the field is missing, and lays the found value out on
// the guest stack for the caller -- invoking it when the metamethod calls for
// that. A chain that does not settle within 100 levels raises the setter loop
// error. The step suspends around its five calls; each gate replays the one its
// resume word names.
void IndexTableValue(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key, uint32_t result) {
  EnterFrame();
  uint32_t object = 0u;
  uint32_t frame = 0u;
  uint32_t call_result = 0u;
  uint32_t metamethod = 0u;
  uint32_t field = 0u;
  uint32_t scratch = 0u;
  uint32_t resume_word = 0u;
  uint32_t walk_count = 0u;
  uint32_t resume_code = 0u;
  uint32_t verdict = 0u;
  uint64_t field_value = 0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kIndexTableValueSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    key = LoadU32(ctx, frame + 8u);
    result = LoadU32(ctx, frame + 12u);
    object = LoadU32(ctx, frame + 16u);
    metamethod = LoadU32(ctx, frame + 20u);
    field = LoadU32(ctx, frame + 24u);
    scratch = LoadU32(ctx, frame + 28u);
    walk_count = LoadU32(ctx, frame + 32u);
    table = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kIndexTableValueResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
    object = LoadU32(ctx, table + 8u);
  }
  // Walk the object's metamethod chain, at most 100 links deep.
  walk_chain:;
  if (ResumePhase(ctx) == kResumeRunning) {
    object = static_cast<uint32_t>(object == 5u);
  }
  if (object != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      object = LoadU32(ctx, table);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == kIndexTableValueResolveCall) {
      call_result = ResolveTableField(ctx, state, object, key);
      resume_code = kIndexTableValueResolveCall;
      if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      field = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadU32(ctx, field + 8u) == 0u) {
        metamethod = LoadU32(ctx, object + 8u);
        if (metamethod != 0u && (LoadU8(ctx, metamethod + 6u) & 2u) == 0u) {
          metamethod = FetchMetamethod(ctx, metamethod, 1u, LoadU32(ctx, LoadU32(ctx, state + 16u) + 192u));
          if (metamethod != 0u) {goto have_metamethod;}
        }
      }
      // No usable metamethod: hand the field itself back to the caller.
      copy_value:;
      StoreU64(ctx, field, LoadU64(ctx, result));
      StoreU32(ctx, field + 8u, LoadU32(ctx, result + 8u));
      StoreU8(ctx, object + 6u, 0u);
      if ((int32_t)LoadU32(ctx, result + 8u) < (int32_t)4u) {goto pop_stack;}
      if ((LoadU8(ctx, LoadU32(ctx, result) + 5u) & 3u) == 0u) {goto pop_stack;}
      if ((LoadU8(ctx, object + 5u) & 4u) == 0u) {goto pop_stack;}
      state = LoadU32(ctx, state + 16u);
      StoreU8(ctx, object + 5u, LoadU8(ctx, object + 5u) & 251u);
      StoreU32(ctx, object + 24u, LoadU32(ctx, state + 40u));
      StoreU32(ctx, state + 40u, object);
      goto pop_stack;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    metamethod = FetchObjectMetamethod(ctx, state, table, 1u);
    object = LoadU32(ctx, metamethod + 8u);
    if (object != 0u) {goto have_metamethod;}
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kIndexTableValueTypeErrorCall) {
    RaiseTypeError(ctx, state, table, kIndexTableValueIndexMessage);
    resume_code = kIndexTableValueTypeErrorCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
  }
  have_metamethod:;
  if (ResumePhase(ctx) == kResumeRunning) {
    object = LoadU32(ctx, metamethod + 8u);
    field = static_cast<uint32_t>(object == 6u);
  }
  if (field != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) != kResumeRunning) {
      // Nothing to lay out on the way back, but the slot has to stay non-zero.
      verdict = table;
    } else {
      object = LoadU32(ctx, state + 8u);
      StoreU64(ctx, object, LoadU64(ctx, metamethod));
      metamethod = LoadU32(ctx, metamethod + 8u);
      StoreU32(ctx, object + 8u, metamethod);
      object = LoadU32(ctx, state + 8u);
      StoreU64(ctx, object + 16u, LoadU64(ctx, table));
      StoreU32(ctx, object + 24u, LoadU32(ctx, table + 8u));
      table = LoadU32(ctx, state + 8u);
      StoreU64(ctx, table + 32u, LoadU64(ctx, key));
      StoreU32(ctx, table + 40u, LoadU32(ctx, key + 8u));
      table = LoadU32(ctx, state + 8u);
      StoreU64(ctx, table + 48u, LoadU64(ctx, result));
      key = LoadU32(ctx, result + 8u);
      StoreU32(ctx, table + 56u, key);
      uint32_t const stack_limit = LoadU32(ctx, state + 28u);
      object = LoadU32(ctx, state + 8u);
      verdict = static_cast<uint32_t>((int32_t)(stack_limit - object) <= (int32_t)64u);
    }
    if (verdict != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_word == kIndexTableValueGrowCall) {
        GrowValueStack(ctx, state, 4u);
        resume_code = kIndexTableValueGrowCall;
        if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        object = LoadU32(ctx, state + 8u);
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, state + 8u, object - kIndexTableValueStackHeadroom);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_word == kIndexTableValueInvokeCall) {
        CallValue(ctx, state, object, 0u);
        resume_code = kIndexTableValueInvokeCall;
        if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      }
      if (ResumePhase(ctx) == kResumeRunning) {goto pop_stack;}
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      field_value = LoadU64(ctx, metamethod);
      StoreU32(ctx, scratch + 8u, object);
      StoreU64(ctx, scratch, field_value);
      table = scratch;
      walk_count = walk_count + 1u;
      metamethod = static_cast<uint32_t>(walk_count != 100u);
      if (metamethod != 0u) {goto walk_chain;}
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kIndexTableValueLoopErrorCall) {
    RaiseScriptError(ctx, state, kIndexTableValueLoopMessage, 0u);
    resume_code = kIndexTableValueLoopErrorCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
  }
  pop_stack:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 16u;
  }
  // The running pass is done; the frame write below belongs to a suspend only.
  goto exit_path;
  // Suspend here: the resume word naming the call in flight goes first, then the
  // nine saved words, and the caller's frame pointer is left above both.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kIndexTableValueResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, key);
  StoreU32(ctx, frame + 12u, result);
  StoreU32(ctx, frame + 16u, object);
  StoreU32(ctx, frame + 20u, metamethod);
  StoreU32(ctx, frame + 24u, field);
  StoreU32(ctx, frame + 28u, scratch);
  StoreU32(ctx, frame + 32u, walk_count);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kIndexTableValueSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}
}  // namespace artc
