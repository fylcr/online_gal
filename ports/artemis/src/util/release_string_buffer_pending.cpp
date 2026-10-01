#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the raw block pool and the panic path.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

// The pending slot in the engine's fixed data: cleared before the buffer is
// released, read back after it, and cleared again.
constexpr uint32_t kReleaseStringBufferPendingSlotAddr = 0xD015Cu;

}  // namespace

// Releases the string buffer a pending slot holds: the slot is cleared and the
// buffer goes back to the pool. If the slot still reads as pending the catch is
// resolved from the current exception instead. Returns nothing.
void ReleaseStringBufferPending(EngineContext* ctx, uint32_t buffer, uint32_t spilled) {
  EnterFrame();
  // The reused slots: the word naming the call the suspend path was taken in,
  // and the catch index the resolved path works with.
  uint32_t resume_word = 0u;
  uint32_t catch_index = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame the suspend path pushed and rebuild the
    // catch index from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    catch_index = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kReleaseStringBufferPendingSlotAddr, 0u);
    DeallocateRawBlock(ctx, buffer);
    // Read back between the two clears -- the release may hand the slot on.
    catch_index = LoadU32(ctx, kReleaseStringBufferPendingSlotAddr);
    StoreU32(ctx, kReleaseStringBufferPendingSlotAddr, 0u);
    if (catch_index != 1u) {
      goto exit_path;
    }
    catch_index = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
    Panic(ctx, catch_index);
    spilled = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_path;
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), spilled);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, catch_index);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
