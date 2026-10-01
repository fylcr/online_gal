#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t CheckComparableTypes(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t CompareScriptStrings(EngineContext* ctx, uint32_t left, uint32_t right);

namespace {

constexpr uint32_t kCompareNotEqual = 0xFFFFFFFFu;  // "strings differ" marker
constexpr uint32_t kCompareMetamethod = 13u;  // the comparison metamethod
constexpr uint32_t kCompareValueBytes = 48u;  // one value record on the stack
constexpr uint32_t kCompareConstantOperandsSaveFrameBytes = 20u;  // saved state words
constexpr uint32_t kCompareConstantOperandsResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Compares two constant operands: equal types are folded here (numbers by
// value, strings byte by byte), anything else goes through the comparison
// metamethod and the value stack.  The steps that grow the stack and call
// back into the script can suspend, so the operands and the stack pointers
// are kept in the frame.
uint32_t CompareConstantOperands(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t type = 0u;       // the operand type, later a data pointer
  uint32_t differs = 0u;    // whether the two operand types differ
  uint32_t slot = 0u;       // the value stack slot the result is built in
  uint32_t base = 0u;       // the bottom of the value stack
  uint32_t result = 0u;     // the comparison result
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCompareConstantOperandsSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    left = LoadU32(ctx, frame + 4u);
    right = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
    base = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCompareConstantOperandsResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Operands of different types cannot be folded without the metamethod.
    type = LoadU32(ctx, left + 8u);
    differs = type != LoadU32(ctx, right + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (differs != 0u) { goto generic_compare; }
    switch (type - 3u) {
      case 0: goto compare_numbers;
      case 1: goto compare_strings;
      default: goto compare_metamethod;
    }
compare_numbers:
    result = LoadF64(ctx, left) < LoadF64(ctx, right);
    goto exit_path;
compare_strings:
    // Both operands are strings: compare their bytes from the front.
    type = LoadU32(ctx, left);
    state = type + 16u;
    right = LoadU32(ctx, right);
    slot = right + 16u;
    left = CompareScriptStrings(ctx, state, slot);
    if (left != 0u) { goto string_done; }
    type = LoadU32(ctx, type + 12u);
    right = LoadU32(ctx, right + 12u);
loop_strings:
    left = StringLength(ctx, state);
    if (right == left) {
      left = 0u;
      goto string_done;
    }
    if (left == type) {
      left = kCompareNotEqual;
      goto string_done;
    }
    // Advance both strings by the common prefix that was compared.
    left = left + 1u;
    right = right - left;
    type = type - left;
    state = state + left;
    slot = slot + left;
    left = CompareScriptStrings(ctx, state, slot);
    if (left == 0u) { goto loop_strings; }
string_done:
    result = left >> 31u;
    goto exit_path;
compare_metamethod:
    // Neither a plain number nor a plain string: use the metamethod when the
    // operands agree on one.
    type = FetchObjectMetamethod(ctx, state, left, kCompareMetamethod);
    if (LoadU32(ctx, type + 8u) == 0u) { goto generic_compare; }
    if (RawValueEqual(ctx, type,
            FetchObjectMetamethod(ctx, state, right, kCompareMetamethod)) == 0u) {
      goto generic_compare;
    }
    // Build the argument list on the value stack: the metamethod, both
    // operands, and the room the call needs.
    base = LoadU32(ctx, state + 32u);
    slot = LoadU32(ctx, state + 8u);
    StoreU64(ctx, slot, LoadU64(ctx, type));
    StoreU32(ctx, slot + 8u, LoadU32(ctx, type + 8u));
    type = LoadU32(ctx, state + 8u);
    StoreU64(ctx, type + 16u, LoadU64(ctx, left));
    StoreU32(ctx, type + 24u, LoadU32(ctx, left + 8u));
    left = LoadU32(ctx, state + 8u);
    StoreU64(ctx, left + 32u, LoadU64(ctx, right));
    StoreU32(ctx, left + 40u, LoadU32(ctx, right + 8u));
    right = LoadU32(ctx, state + 28u);
    left = LoadU32(ctx, state + 8u);
    right = (int32_t)(right - left) <= 48;
  }
  if (right != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // Room for the call the metamethod comparison makes.
      GrowValueStack(ctx, state, 3u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The argument list sits at the current stack top; a resumed pass keeps the
      // slot the frame restored.
      left = LoadU32(ctx, state + 8u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The call's value slot is reserved on the stack.
    right = left + kCompareValueBytes;
    StoreU32(ctx, state + 8u, right);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    CallValue(ctx, state, left, 1u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The returned value is moved down into the slot that was reserved.
    left = LoadU32(ctx, state + 8u) - 16u;
    StoreU32(ctx, state + 8u, left);
    right = LoadU32(ctx, state + 32u) + (slot - base);
    StoreU64(ctx, right, LoadU64(ctx, left));
    StoreU32(ctx, right + 8u, LoadU32(ctx, left + 8u));
    state = LoadU32(ctx, state + 8u);
    left = LoadU32(ctx, state + 8u);
    switch (left) {
      case 0: goto finished;
      case 1: goto result_true;
      default: goto result_value;
    }
result_true:
    result = LoadU32(ctx, state) != 0u;
    goto exit_path;
result_value:
    result = 1u;
    goto exit_path;
  }
generic_compare:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    type = CheckComparableTypes(ctx, state, left, right);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    left = type;
  }
finished:
  if (ResumePhase(ctx) == kResumeRunning) {
    result = left;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCompareConstantOperandsResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, left);
    StoreU32(ctx, frame + 8u, right);
    StoreU32(ctx, frame + 12u, slot);
    StoreU32(ctx, frame + 16u, base);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCompareConstantOperandsSaveFrameBytes);
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
