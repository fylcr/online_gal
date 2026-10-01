#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void Panic(EngineContext* ctx, uint32_t a0);
void ResizeValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void PushTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);

namespace {

constexpr uint32_t kRegistryPendingFlag = 0xD015Cu;  // the pending-call flag's guest address
constexpr uint32_t kRegistryWaitTag = 0xFFFFD8F0u;                   // wait tag handed to the sync host calls
constexpr uint32_t kRegistryResizeTag = 0xFFFFFFFDu;                 // value-stack resize tag for one slot
constexpr uint32_t kRegistryEmptyValue = 0xFFFFFFFEu;                // tag marking an empty value slot
constexpr uint32_t kRegistryStackTopAll = 0xFFFFFFFFu;               // SetStackTop index meaning "all of it"
constexpr uint32_t kRegistryFrameBytes = 24u;                        // saved frame: six words
constexpr uint32_t kRegistryResumeWordBytes = 4u;                    // resume word written before it
constexpr uint64_t kRegistryEmptyPair = 0xFFFFFFFE00000000ull;       // marks both halves of a slot pair empty

}  // namespace

// Reads the registry pseudo-table field named by `b` into the two value slots
// held by `a`: the pair is marked empty, whatever it held is released, and the
// read is retried while the registry is still locked.
uint32_t GetRegistryTableField(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t value = 0u;
  uint32_t frame = 0u;
  uint32_t call_result = 0u;
  uint32_t entry = 0u;
  uint32_t resume_point = 0u;
  uint32_t pending = 0u;
  uint32_t saved = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kRegistryFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 8u);
    entry = LoadU32(ctx, frame + 12u);
    pending = LoadU32(ctx, frame + 16u);
    saved = LoadU32(ctx, frame + 20u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kRegistryResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Take the key's current value, empty both slots, then start the read.
    entry = LoadU32(ctx, b);
    StoreU64(ctx, a + 12u, kRegistryEmptyPair);
    StoreU64(ctx, a + 4u, kRegistryEmptyPair);
    StoreU32(ctx, a, entry);
    b = LoadU32(ctx, b + 4u);
    StoreU32(ctx, kDataD015C, 0u);
    PushTableField(ctx, entry, kRegistryWaitTag, b);
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    b = (uint32_t)(value == 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // If the push already finished the value is in place; otherwise drop the
    // value the field still holds and try once more.
    if (b) goto take_pending_result;
    b = LoadU32(ctx, a);
    StoreU32(ctx, kDataD015C, 0u);
    value = LoadU32(ctx, b + 8u);
    StoreU32(ctx, value + 8u, 0u);
    value = value + 16u;
    StoreU32(ctx, b + 8u, value);
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    b = (uint32_t)(b == 1u);
    if (b) goto take_pending_result;
    b = LoadU32(ctx, a);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    call_result = HostOsQuery(ctx, 987u, b, kRegistryEmptyValue);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    b = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    value = (uint32_t)(value == 1u);
    if (value) goto take_pending_result;
    b = !b;
    if (b) goto clear_result;
    b = LoadU32(ctx, a);
    StoreU32(ctx, kDataD015C, 0u);
    SetStackTop(ctx, b, kRegistryEmptyValue);
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    value = (uint32_t)(value == 1u);
    if (value) goto take_pending_result_and_resize;
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    call_result = HostOsQuery(ctx, 1001u, b, kRegistryWaitTag);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    pending = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    value = (uint32_t)(value == 1u);
    if (value) goto take_pending_result_and_resize;
    value = LoadU32(ctx, a + 12u);
    StoreU32(ctx, a + 12u, b);
    saved = LoadU32(ctx, a + 16u);
    StoreU32(ctx, a + 16u, pending);
    pending = !value;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (pending) goto store_pending_slots;
    if (saved == kRegistryEmptyValue) goto store_pending_slots;
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    HostSyncSlot(ctx, 1002u, value, kRegistryWaitTag, saved);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    value = (uint32_t)(value == 1u);
    if (value) goto wait_failed;
  }
store_pending_slots:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Hand the slots the field's old values, then retry the read.
    StoreU32(ctx, kDataD015C, 0u);
    ResizeValueStack(ctx, b, kRegistryResizeTag);
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (b == 1u) goto wait_failed;
    b = LoadU32(ctx, a);
    StoreU32(ctx, kDataD015C, 0u);
    SetStackTop(ctx, b, kRegistryStackTopAll);
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    value = (uint32_t)(value == 1u);
    if (value) goto take_result;
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    call_result = HostOsQuery(ctx, 1001u, b, kRegistryWaitTag);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    saved = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    value = (uint32_t)(value == 1u);
    if (value) goto take_result;
    value = LoadU32(ctx, a + 4u);
    StoreU32(ctx, a + 4u, b);
    b = LoadU32(ctx, a + 8u);
    StoreU32(ctx, a + 8u, saved);
    if (!value) goto resize_result_slots;
    if (b == kRegistryEmptyValue) goto resize_result_slots;
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    HostSyncSlot(ctx, 1002u, value, kRegistryWaitTag, b);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    b = (uint32_t)(b != 1u);
    if (b) goto resize_result_slots;
    goto wait_failed;
  }
take_pending_result:
  if (ResumePhase(ctx) == kResumeRunning) {
    value = HostRet0(ctx);
    b = HostTempRet0(ctx);
    goto resize_slots;
  }
take_pending_result_and_resize:
  if (ResumePhase(ctx) == kResumeRunning) {
    value = HostRet0(ctx);
    HostTempRet0(ctx);
    StoreU32(ctx, kDataD015C, 0u);
    ResizeValueStack(ctx, b, kRegistryResizeTag);
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    b = (uint32_t)(b != 1u);
    if (b) goto resize_slots;
    goto wait_failed;
  }
clear_result:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, a, 0u);
  }
resize_result_slots:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kDataD015C, 0u);
    ResizeValueStack(ctx, entry, kRegistryEmptyValue);
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (b == 1u) goto wait_failed;
    LeaveFrame();
    return a;
  }
take_result:
  if (ResumePhase(ctx) == kResumeRunning) {
    value = HostRet0(ctx);
    b = HostTempRet0(ctx);
  }
resize_slots:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kDataD015C, 0u);
    ResizeValueStack(ctx, entry, kRegistryEmptyValue);
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (b == 1u) goto wait_failed;
    b = LoadU32(ctx, a + 12u);
    entry = !b;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (entry) goto check_second_slot;
    entry = LoadU32(ctx, a + 16u);
    if (entry == kRegistryEmptyValue) goto check_second_slot;
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    HostSyncSlot(ctx, 1002u, b, kRegistryWaitTag, entry);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    b = (uint32_t)(b == 1u);
    if (b) goto wait_failed;
  }
check_second_slot:
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, a + 4u);
    entry = !b;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (entry) goto release_guard;
    a = LoadU32(ctx, a + 8u);
    if (a == kRegistryEmptyValue) goto release_guard;
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    HostSyncSlot(ctx, 1002u, b, kRegistryWaitTag, a);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    a = (uint32_t)(a == 1u);
    if (a) goto wait_failed;
  }
release_guard:
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, value);
    TrapUnreachable();
  }
wait_failed:
  if (ResumePhase(ctx) == kResumeRunning) {
    a = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    Panic(ctx, a);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  TrapUnreachable();
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kRegistryResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, b);
  StoreU32(ctx, frame + 8u, value);
  StoreU32(ctx, frame + 12u, entry);
  StoreU32(ctx, frame + 16u, pending);
  StoreU32(ctx, frame + 20u, saved);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kRegistryFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
