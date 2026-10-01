#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

constexpr uint32_t kTypeErrorFormattedMessage = 0x2A718u;  // "attempt to %s %s '%s' (a %s value)"
constexpr uint32_t kTypeErrorPlainMessage = 0x2A73Bu;  // "attempt to %s a %s value"
constexpr uint32_t kTypeErrorTypeNameTable = 0x2BD80u;  // base of the table of type-name strings
constexpr uint32_t kTypeErrorSaveFrameBytes = 20u;  // args, scratch pointer, type name
constexpr uint32_t kTypeErrorResumeWordBytes = 4u;  // resume code slot below the saved state

}  // namespace

// Reports a type error: the operand's type name and the kind of variable it
// denotes are formatted into the message.  Suspendable: the error reporter can
// suspend, so every value that outlives it is kept in the frame below.
void RaiseTypeError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t scratch_frame = 0u;
  uint32_t type_name = 0u;
  uint32_t cursor = 0u;
  uint32_t resume_point = 0u;
  uint32_t entry_list = 0u;
  uint32_t entry_end = 0u;
  uint32_t list_empty = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kTypeErrorSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    b = LoadU32(ctx, frame + 4u);
    c = LoadU32(ctx, frame + 8u);
    scratch_frame = LoadU32(ctx, frame + 12u);
    type_name = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kTypeErrorResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 48-byte scratch area that carries the message arguments.
    scratch_frame = GuestStackSp(ctx) - 48u;
    GuestStackSp(ctx) = scratch_frame;
    StoreU32(ctx, scratch_frame + 44u, 0u);  // variable kind, filled in by the resolver
    entry_list = LoadU32(ctx, a + 20u);
    cursor = LoadU32(ctx, entry_list);
    entry_end = LoadU32(ctx, entry_list + 8u);
    list_empty = cursor >= entry_end;
    // The operand's type word indexes the table of type-name strings.
    type_name = LoadU32(ctx, (LoadU32(ctx, b + 8u) << 2u) + kTypeErrorTypeNameTable);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (list_empty != 0u) goto describe_operand;
  find_entry:
    if (b != cursor) {
      cursor += 16u;
      if (entry_end > cursor) goto find_entry;
      goto describe_operand;
    }
    // The operand still points at a live entry: resolve the variable it names.
    b = ResolveVariableKind(ctx, a, entry_list,
                            (uint32_t)((int32_t)(b - LoadU32(ctx, a + 12u)) >> 4),
                            scratch_frame + 44u);
    if (b == 0u) goto describe_operand;
    StoreU32(ctx, scratch_frame + 16u, c);
    StoreU32(ctx, scratch_frame + 20u, b);
    StoreU32(ctx, scratch_frame + 24u, LoadU32(ctx, scratch_frame + 44u));
    StoreU32(ctx, scratch_frame + 28u, type_name);
    b = scratch_frame + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    RaiseScriptError(ctx, a, kTypeErrorFormattedMessage, b);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto reclaim_scratch; }
describe_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The short message reuses the first two slots of the same buffer.
    StoreU32(ctx, scratch_frame + 4u, type_name);
    StoreU32(ctx, scratch_frame, c);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    RaiseScriptError(ctx, a, kTypeErrorPlainMessage, scratch_frame);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
reclaim_scratch:
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch_frame + 48u;
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kTypeErrorResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, c);
    StoreU32(ctx, frame + 12u, scratch_frame);
    StoreU32(ctx, frame + 16u, type_name);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kTypeErrorSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
