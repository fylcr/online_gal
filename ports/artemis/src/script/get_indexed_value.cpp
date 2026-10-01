#include "util/engine_context.h"

namespace artc {

// The callees this reader reaches: the table's own field lookup
// (script/lookup_table_field.cpp), the two event lookups a type can offer, the
// value stack's grower, the guest caller, and the diagnostic a type error is
// raised with. RaiseScriptError needs no declaration here — the shared
// vocabulary header already declares it.
uint32_t LookupTableField(EngineContext* ctx, uint32_t table, uint32_t key);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t metatable, uint32_t event,
                         uint32_t event_name);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t state, uint32_t object,
                               uint32_t event);
void GrowValueStack(EngineContext* ctx, uint32_t state, uint32_t slots);
void CallValue(EngineContext* ctx, uint32_t state, uint32_t function_base,
               uint32_t want_result);
void RaiseTypeError(EngineContext* ctx, uint32_t state, uint32_t object, uint32_t varname);

namespace {

// The event this reader consults; the engine numbers its index event zero.
constexpr uint32_t kIndexEvent = 0u;
// The guest's type tag for a table-shaped value, and for a function value.
constexpr uint32_t kTableTag = 5u;
constexpr uint32_t kFunctionTag = 6u;
// How many times the walk may hand the read on before the engine calls it a
// loop.
constexpr uint32_t kChainLimit = 100u;
// Three 16-byte TValues staged for the event call, and the room they take.
constexpr uint32_t kStagedBytes = 48u;
// The two diagnostics this body can raise. Both sit in the guest's string pool
// beside the shared nil record, so they are named off that anchor instead of as
// raw pool addresses.
constexpr uint32_t kIndexTextAddr = kNilValueAddr + 2999u;
constexpr uint32_t kLoopTextAddr = kNilValueAddr + 3005u;
// The codes a suspending frame spills to name the call it was inside.
constexpr uint32_t kTypeErrorCall = 0u;
constexpr uint32_t kGrowCall = 1u;
constexpr uint32_t kEventCall = 2u;
constexpr uint32_t kLoopErrorCall = 3u;

}  // namespace

void GetIndexedValue(EngineContext* ctx, uint32_t state, uint32_t target, uint32_t key,
                     uint32_t out_record) {
  EnterFrame();
  // One slot carries the event record the walk finds, and later the base of the
  // staged call; another carries the target's tag, and later the answer the
  // lookup produced; a third carries the walk's step count, and later the frame
  // base. They are all declared up here because the labels below are jumped over
  // and because a rewinding pass restores them from the frame.
  uint32_t scratch = 0u;
  uint32_t resume_code = 0u;
  uint32_t chain_steps = 0u;
  uint32_t probe = 0u;
  uint32_t stage_gate = 0u;
  uint32_t stack_limit = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspension saved and rebuild every
    // slot from it — the four parameters first, then the three reused locals,
    // in the order they were pushed. The step count is the last word read, so
    // the frame pointer is only given up once every other word has been taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 28u);
    chain_steps = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, chain_steps);
    target = LoadU32(ctx, chain_steps + 4u);
    key = LoadU32(ctx, chain_steps + 8u);
    out_record = LoadU32(ctx, chain_steps + 12u);
    scratch = LoadU32(ctx, chain_steps + 16u);
    probe = LoadU32(ctx, chain_steps + 20u);
    chain_steps = LoadU32(ctx, chain_steps + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the word naming the call this frame was inside.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the target's type tag is read once, before the walk, because the
    // walk head below folds it into its table-shaped latch.
    probe = LoadU32(ctx, target + 8u);
  }
chain_step:;
  // The latch is recomputed on a running pass and restored on a rewinding one,
  // because the walk reuses the same slot for the event record it finds.
  scratch = ResumePhase(ctx) ? scratch : static_cast<uint32_t>(probe == kTableTag);
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scratch) {
      // Table-shaped target: probe the store, and answer with the record as
      // soon as it holds a non-nil one. A missing metatable, or one whose
      // cached no-event flag is set, answers with the record the probe produced
      // as well — the shared nil record when the field is absent.
      scratch = LoadU32(ctx, target);
      probe = LookupTableField(ctx, scratch, key);
      if (LoadU32(ctx, probe + 8u) != 0u) {
        goto return_record;
      }
      scratch = LoadU32(ctx, scratch + 8u);
      if (scratch == 0u) {
        goto return_record;
      }
      if ((LoadU8(ctx, scratch + 6u) & 1u) != 0u) {
        goto return_record;
      }
      {
        uint32_t const event_name = LoadU32(ctx, LoadU32(ctx, state + 16u) + 188u);
        scratch = FetchMetamethod(ctx, scratch, kIndexEvent, event_name);
      }
      if (scratch != 0u) {
        goto dispatch_metamethod;
      }
    return_record:;
      StoreU64(ctx, out_record, LoadU64(ctx, probe));
      StoreU32(ctx, out_record + 8u, LoadU32(ctx, probe + 8u));
      goto exit_path;
    }
    // Any other shape: ask the target's own type for the event. A nil answer
    // falls through to the type error below.
    scratch = FetchObjectMetamethod(ctx, state, target, kIndexEvent);
    probe = LoadU32(ctx, scratch + 8u);
    if (probe != 0u) {
      goto dispatch_metamethod;
    }
  }
  // A target that is neither table-shaped nor the owner of an index event is a
  // guest type error, reported with the pool's own "index" text. The code a
  // suspension spills here is zero, so a rewinding frame re-runs this report.
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTypeErrorCall) : 1u) {
    RaiseTypeError(ctx, state, target, kIndexTextAddr);
    resume_code = kTypeErrorCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
dispatch_metamethod:;
  // The event is callable only when its tag says function; otherwise the walk
  // hands it on as the next target and repeats, up to the engine's limit.
  if (ResumePhase(ctx) == kResumeRunning) {
    probe = LoadU32(ctx, scratch + 8u);
    if (probe != kFunctionTag) {
      target = scratch;
      chain_steps = chain_steps + 1u;
      scratch = static_cast<uint32_t>(chain_steps == kChainLimit);
      if (scratch) {
        goto chain_limit;
      }
      goto chain_step;
    }
  }
  // Staging for the event call. On a running pass the three 16-byte TValues go
  // at the value-stack top — the event record, then the target, then the key —
  // and the room left there is measured against the bytes they take. On a
  // rewinding pass the frame base comes back from the frame instead, which is
  // why one slot carries both.
  if (ResumePhase(ctx)) {
    stage_gate = target;
  } else {
    probe = LoadU32(ctx, state + 32u);
    chain_steps = LoadU32(ctx, state + 8u);
    StoreU64(ctx, chain_steps, LoadU64(ctx, scratch));
    StoreU32(ctx, chain_steps + 8u, LoadU32(ctx, scratch + 8u));
    scratch = LoadU32(ctx, state + 8u);
    StoreU64(ctx, scratch + 16u, LoadU64(ctx, target));
    StoreU32(ctx, scratch + 24u, LoadU32(ctx, target + 8u));
    target = LoadU32(ctx, state + 8u);
    StoreU64(ctx, target + 32u, LoadU64(ctx, key));
    StoreU32(ctx, target + 40u, LoadU32(ctx, key + 8u));
    stack_limit = LoadU32(ctx, state + 28u);
    scratch = LoadU32(ctx, state + 8u);
    stage_gate = static_cast<uint32_t>(
        static_cast<int32_t>(stack_limit - scratch) <= static_cast<int32_t>(kStagedBytes));
  }
  stage_gate |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (stage_gate) {
    // Not enough room for the three slots — or a rewinding pass replaying the
    // room-making call itself. The code a suspension spills here is one.
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kGrowCall) : 1u) {
      GrowValueStack(ctx, state, 3u);
      resume_code = kGrowCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
    }
    // The top past the three staged records: on a running pass it is that many
    // bytes beyond the base they were written at, and on a rewinding pass the
    // frame already carries it.
    scratch = ResumePhase(ctx) ? scratch : LoadU32(ctx, state + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, state + 8u, scratch + kStagedBytes);
  }
  // The call itself, over the three staged records; the last argument asks for
  // a single result. The code a suspension spills here is two.
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEventCall) : 1u) {
    CallValue(ctx, state, scratch, 1u);
    resume_code = kEventCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: give the caller its record. The base slot of the staged call is
    // released first, so the answer is the one record the call left at the top;
    // the caller's slot is relocated by whatever the call did to the value-stack
    // base published at state+32.
    uint32_t const frame_base = LoadU32(ctx, state + 8u) - 16u;
    StoreU32(ctx, state + 8u, frame_base);
    uint32_t const dest = LoadU32(ctx, state + 32u) + (out_record - probe);
    StoreU64(ctx, dest, LoadU64(ctx, frame_base));
    StoreU32(ctx, dest + 8u, LoadU32(ctx, frame_base + 8u));
    goto exit_path;
  }
chain_limit:;
  // The walk ran the engine's limit without finding either a record or a
  // callable. The code a suspension spills here is three.
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kLoopErrorCall) : 1u) {
    RaiseScriptError(ctx, state, kLoopTextAddr, kIndexEvent);
    resume_code = kLoopErrorCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
  goto exit_path;
suspend_frame:;
  {
    // Suspend: spill the code naming the call in flight, step past it, then
    // spill the frame a rewinding pass reads back — the four parameters and the
    // three reused locals.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, target);
    StoreU32(ctx, saved_frame + 8u, key);
    StoreU32(ctx, saved_frame + 12u, out_record);
    StoreU32(ctx, saved_frame + 16u, scratch);
    StoreU32(ctx, saved_frame + 20u, probe);
    StoreU32(ctx, saved_frame + 24u, chain_steps);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 28u);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
