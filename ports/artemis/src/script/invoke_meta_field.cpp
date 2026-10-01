#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kInvokeMetaFieldSaveFrameBytes = 20u;  // saved words below the resume code
constexpr uint32_t kInvokeMetaFieldResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Resolves the metamethod a field access needs and calls it with the field's
// value.  The call can suspend, so the object, the value record and the
// object's saved field word are kept in the frame.
void InvokeMetaField(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t value = 0u;        // the projected value, then its new record
  uint32_t object = 0u;       // the object the metamethod is looked up on
  uint32_t saved_field = 0u;  // the object's field word, saved around the call
  uint32_t saved_flag = 0u;   // the state flag byte, saved around the call
  uint32_t entry = 0u;        // the object's current metamethod entry
  uint32_t payload = 0u;      // the entry's payload object
  uint32_t missing = 0u;      // set when the value carries no payload
  uint32_t metamethod = 0u;   // the metamethod to invoke
  uint32_t record = 0u;       // the 32-byte value record built on the guest stack
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kInvokeMetaFieldSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
    object = LoadU32(ctx, frame + 8u);
    saved_field = LoadU32(ctx, frame + 12u);
    saved_flag = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kInvokeMetaFieldResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Unlink the value's node from the object's entry chain.
    object = LoadU32(ctx, a + 16u);
    entry = LoadU32(ctx, object + 48u);
    value = LoadU32(ctx, entry);
    if (entry == value) {
      StoreU32(ctx, object + 48u, 0u);
      goto copy_done;
    }
    StoreU32(ctx, entry, LoadU32(ctx, value));
copy_done:
    StoreU32(ctx, value, LoadU32(ctx, LoadU32(ctx, object + 112u)));
    StoreU32(ctx, LoadU32(ctx, object + 112u), value);
    StoreU8(ctx, value + 5u,
        (LoadU8(ctx, object + 20u) & 3u) | (LoadU8(ctx, value + 5u) & 248u));
    payload = LoadU32(ctx, value + 8u);
    missing = payload == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Nothing to invoke: no payload, or the payload carries no metamethod.
    if (missing != 0u) { goto exit_path; }
    if ((LoadU8(ctx, payload + 6u) & 4u) != 0u) { goto exit_path; }
    metamethod = FetchMetamethod(ctx, payload, 2u, LoadU32(ctx, LoadU32(ctx, a + 16u) + 196u));
    if (metamethod == 0u) { goto exit_path; }
    // Build the 32-byte value record the metamethod is called with, and
    // remember the field the running call temporarily clears.
    saved_field = LoadU32(ctx, object + 64u);
    saved_flag = LoadU8(ctx, a + 57u);
    StoreU8(ctx, a + 57u, 0u);
    StoreU32(ctx, object + 64u, LoadU32(ctx, object + 68u) << 1u);
    record = LoadU32(ctx, a + 8u);
    StoreU64(ctx, record, LoadU64(ctx, metamethod));
    StoreU32(ctx, record + 8u, LoadU32(ctx, metamethod + 8u));
    record = LoadU32(ctx, a + 8u);
    StoreU32(ctx, record + 16u, value);
    StoreU32(ctx, record + 24u, 7u);
    value = LoadU32(ctx, a + 8u);
    StoreU32(ctx, a + 8u, value + 32u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // CallValue can suspend, so the values that outlive it are in the frame.
    CallValue(ctx, a, value, 0u);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The call is done: put the field back and drop the record.
    StoreU8(ctx, a + 57u, saved_flag);
    StoreU32(ctx, object + 64u, saved_field);
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kInvokeMetaFieldResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, value);
    StoreU32(ctx, frame + 8u, object);
    StoreU32(ctx, frame + 12u, saved_field);
    StoreU32(ctx, frame + 16u, saved_flag);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kInvokeMetaFieldSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
