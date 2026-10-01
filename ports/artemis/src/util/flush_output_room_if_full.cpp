#include "util/engine_context.h"

namespace artc {

// Forward declarations -- defined in util/ensure_table_array_room.cpp and
// script/format_number.cpp.
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);
uint32_t FormatNumber(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);

// Makes room in the fixed output room hung off the state before another chunk is
// written: the room is asked whether it is full, and when it is, the table array
// is grown and the pending number is flushed. Answers the state word, or zero
// when the step suspended.
uint32_t FlushOutputRoomIfFull(EngineContext* ctx, uint32_t state, uint32_t limit, uint32_t chunk) {
  EnterFrame();
  // The reused slots: the stack pointer the suspend path saved and hands back,
  // the resume word naming the call in flight, and the answer.
  uint32_t stack_pointer = 0u;
  uint32_t resume_word = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path pushed and rebuild the
    // state, the chunk, the stack pointer and the limit from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    chunk = LoadU32(ctx, saved_frame + 8u);
    stack_pointer = LoadU32(ctx, saved_frame + 12u);
    limit = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // Only a running pass takes the 16-byte slab of guest stack and asks the room;
  // a rewinding pass is covered by the bit below.
  uint32_t room_full = 0u;
  if (ResumePhase(ctx) == kResumeRunning) {
    stack_pointer = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = stack_pointer;
    uint32_t const room = LoadU32(ctx, state + 16u);
    uint32_t const written = LoadU32(ctx, room + 68u);
    room_full = static_cast<uint32_t>(written >= LoadU32(ctx, room + 64u));
  }
  // The rewinding bit is orred into the full flag rather than tested beside it.
  if (room_full | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      EnsureTableArrayRoom(ctx, state);
      result = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: park the chunk in the slab's last word before the flush.
    StoreU32(ctx, stack_pointer + 12u, chunk);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 1u) : 1u) {
    // The flush's answer only becomes the state once the call has returned
    // without suspending.
    uint32_t const formatted = FormatNumber(ctx, state, limit, chunk);
    result = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    state = formatted;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running and done: give the slab back and answer the state.
    GuestStackSp(ctx) = stack_pointer + 16u;
    result = state;
    goto exit_path;
  }
  TrapUnreachable();
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), result);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, limit);
    StoreU32(ctx, saved_frame + 8u, chunk);
    StoreU32(ctx, saved_frame + 12u, stack_pointer);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  }
  result = 0u;
 exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
