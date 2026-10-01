#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);

// Compiles a comparison of two operands.  A tag that already carries the answer
// folds to it, the numeric tags compare as machine words or as doubles, and the
// remaining tags run the pair's comparison metamethod, which is called only when
// both operands expose the same one.  Returns nonzero when the comparison
// produced a value.  Suspendable: the stack growth and the metamethod call can
// both suspend.
uint32_t CompileComparison(EngineContext* ctx, uint32_t func_state, uint32_t lhs, uint32_t rhs) {
  EnterFrame();
  uint32_t tag = 0u;
  uint32_t operand = 0u;
  uint32_t peer = 0u;
  uint32_t type_object = 0u;
  uint32_t stack_base = 0u;
  uint32_t metamethod_table = 0u;
  uint32_t limit = 0u;
  // The result flag; on the metamethod path this word holds the stack slot the
  // operand pair is copied to.
  uint32_t result = 0u;
  uint32_t resume_point = 0u;
  double left_number = 0;
  double right_number = 0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 20u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    rhs = LoadU32(ctx, frame + 8u);
    tag = LoadU32(ctx, frame + 12u);
    result = LoadU32(ctx, frame + 16u);
    lhs = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A comparison is assumed to produce a value until a tag says otherwise.
    result = 1u;
    tag = LoadU32(ctx, lhs + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (tag) {
      case 0:
        // No operand to compare: nothing is produced.
        goto no_result;
      case 3:
        goto compare_numbers;
      case 5:
        goto via_metamethod_b;
      case 7:
        goto via_metamethod_a;
      default:
        goto compare_words;
    }
    compare_numbers:;
    left_number = LoadF64(ctx, lhs);
    right_number = LoadF64(ctx, rhs);
    LeaveFrame();
    return static_cast<uint32_t>(left_number == right_number);
    // Tags 5 and 7 hand the pair to the comparison metamethod.  Both blocks do
    // the same work and differ only in how they leave it.
    via_metamethod_a:;
    // Distinct operands, both with a type object, neither flagged as
    // metatable-less, and both exposing the same comparison metamethod.
    operand = LoadU32(ctx, lhs);
    peer = LoadU32(ctx, rhs);
    if (operand == peer) { goto no_result; }
    result = 0u;
    type_object = LoadU32(ctx, operand + 8u);
    if (type_object == 0u) { goto no_result; }
    if (LoadU8(ctx, type_object + 6u) & 16u) { goto no_result; }
    peer = LoadU32(ctx, peer + 8u);
    metamethod_table = LoadU32(ctx, LoadU32(ctx, func_state + 16u) + 204u);
    operand = FetchMetamethod(ctx, type_object, 4u, metamethod_table);
    if (operand == 0u) { goto no_result; }
    if (peer == type_object) { goto run_metamethod; }
    if (peer == 0u) { goto no_result; }
    if (LoadU8(ctx, peer + 6u) & 16u) { goto no_result; }
    metamethod_table = LoadU32(ctx, LoadU32(ctx, func_state + 16u) + 204u);
    peer = FetchMetamethod(ctx, peer, 4u, metamethod_table);
    if (peer == 0u) { goto no_result; }
    if (RawValueEqual(ctx, operand, peer)) { goto run_metamethod; }
    goto no_result;
    via_metamethod_b:;
    operand = LoadU32(ctx, lhs);
    peer = LoadU32(ctx, rhs);
    if (operand == peer) { goto no_result; }
    result = 0u;
    type_object = LoadU32(ctx, operand + 8u);
    if (type_object == 0u) { goto no_result; }
    if (LoadU8(ctx, type_object + 6u) & 16u) { goto no_result; }
    peer = LoadU32(ctx, peer + 8u);
    metamethod_table = LoadU32(ctx, LoadU32(ctx, func_state + 16u) + 204u);
    operand = FetchMetamethod(ctx, type_object, 4u, metamethod_table);
    if (operand == 0u) { goto no_result; }
    if (peer == type_object) { goto run_metamethod; }
    if (peer == 0u) { goto no_result; }
    if (LoadU8(ctx, peer + 6u) & 16u) { goto no_result; }
    metamethod_table = LoadU32(ctx, LoadU32(ctx, func_state + 16u) + 204u);
    peer = FetchMetamethod(ctx, peer, 4u, metamethod_table);
    if (peer == 0u) { goto no_result; }
    if (!RawValueEqual(ctx, operand, peer)) { goto no_result; }
    run_metamethod:;
    // Put both operand pairs on the value stack for the call, and check that
    // the frame the call pushes still has room below the stack limit.
    stack_base = LoadU32(ctx, func_state + 32u);
    result = LoadU32(ctx, func_state + 8u);
    StoreU64(ctx, result, LoadU64(ctx, operand));
    StoreU32(ctx, result + 8u, LoadU32(ctx, operand + 8u));
    operand = LoadU32(ctx, func_state + 8u);
    StoreU64(ctx, operand + 16u, LoadU64(ctx, lhs));
    StoreU32(ctx, operand + 24u, LoadU32(ctx, lhs + 8u));
    lhs = LoadU32(ctx, func_state + 8u);
    StoreU64(ctx, lhs + 32u, LoadU64(ctx, rhs));
    StoreU32(ctx, lhs + 40u, LoadU32(ctx, rhs + 8u));
    limit = LoadU32(ctx, func_state + 28u);
    lhs = LoadU32(ctx, func_state + 8u);
    rhs = static_cast<uint32_t>(static_cast<int32_t>(limit - lhs) <= 48);
  }
  if (rhs != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The stack growth is issued unless a rewind already went past it.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      GrowValueStack(ctx, func_state, 3u);
      if (ResumePhase(ctx) == kResumeSuspended) {
        resume_point = 0u;
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // On a rewind the restored operand stands; on the running pass the slot
      // the pair goes to comes from the stack.
      lhs = LoadU32(ctx, func_state + 8u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the 48-byte call frame.
    rhs = lhs + 48u;
    StoreU32(ctx, func_state + 8u, rhs);
  }
  // The metamethod call is issued unless a rewind already went past it.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    CallValue(ctx, func_state, lhs, 1u);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_point = 1u;
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Pop the 16-byte result pair the call left behind and copy it to the slot
    // the comparison is written to.
    lhs = LoadU32(ctx, func_state + 8u) - 16u;
    StoreU32(ctx, func_state + 8u, lhs);
    rhs = LoadU32(ctx, func_state + 32u) + (result - tag);
    StoreU64(ctx, rhs, LoadU64(ctx, lhs));
    StoreU32(ctx, rhs + 8u, LoadU32(ctx, lhs + 8u));
    result = 0u;
    func_state = LoadU32(ctx, func_state + 8u);
    lhs = LoadU32(ctx, func_state + 8u);
    switch (lhs) {
      case 0:
        goto no_result;
      case 1:
        goto use_metamethod_result;
      default:
        goto mark_compared;
    }
    use_metamethod_result:;
    LeaveFrame();
    return static_cast<uint32_t>(LoadU32(ctx, func_state) != 0u);
    mark_compared:;
    result = 1u;
  }
  no_result:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return result;
  }
  compare_words:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return static_cast<uint32_t>(LoadU32(ctx, lhs) == LoadU32(ctx, rhs));
  }
  TrapUnreachable();
  suspend_point:;
  // Suspend: push the resume point, then the 20-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, func_state);
    StoreU32(ctx, frame + 4u, lhs);
    StoreU32(ctx, frame + 8u, rhs);
    StoreU32(ctx, frame + 12u, tag);
    StoreU32(ctx, frame + 16u, result);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 20u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
