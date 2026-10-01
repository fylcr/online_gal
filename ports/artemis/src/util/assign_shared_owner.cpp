#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void CreateSharedRecord(EngineContext* ctx, uint32_t state, uint32_t value);

namespace {

// Guest word that holds the pending-call flag.
constexpr uint32_t kAssignSharedOwnerPendingFlag = 0xD015Cu;
// Bytes of live state pushed on the return frame when the step suspends.
constexpr uint32_t kAssignSharedOwnerSaveFrameBytes = 24u;
// Bytes of resume word pushed above that frame.
constexpr uint32_t kAssignSharedOwnerResumeWordBytes = 4u;
// Guest stack bytes the step borrows as its staging block.
constexpr uint32_t kAssignSharedOwnerScratchBytes = 16u;
// Assertion text at the failure site: "px != 0".
constexpr uint32_t kData91399 = 0x91399u;
// Assertion text at the failure site: ".../boost/smart_ptr/shared_ptr.hpp".
constexpr uint32_t kData913A1 = 0x913A1u;
// Assertion text at the failure site: "operator->".
constexpr uint32_t kData913D9 = 0x913D9u;

}  // namespace

// Gives a value a shared owner: the owner record is linked into the owner's slot
// chain, and the value itself is copied through a 16-byte staging block borrowed
// from the guest stack.  A copy longer than one slot hands its tail back to this
// same step, which moves it through CopyBytes one chunk at a time.
//
// Every callee here can suspend, so each call site carries a resume point naming
// it, and owner, value, shared, field, field_value and scratch are the live words
// pushed before the step returns; the two rewind gates at the top restore them.
void AssignSharedOwner(EngineContext* ctx, uint32_t owner, uint32_t value, uint32_t shared) {
  EnterFrame();
  // field and field_value are the two words of the owner's slot entry; scratch is
  // the block borrowed from the guest stack, staging holds an intermediate
  // offset, and resume_point names the call being resumed.
  uint32_t field = 0u;
  uint32_t field_value = 0u;
  uint32_t staging = 0u;
  uint32_t scratch = 0u;
  uint32_t resume_point = 0u;

  // Rewind: drop the 24-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kAssignSharedOwnerSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    owner = LoadU32(ctx, frame);
    shared = LoadU32(ctx, frame + 8u);
    field = LoadU32(ctx, frame + 12u);
    field_value = LoadU32(ctx, frame + 16u);
    scratch = LoadU32(ctx, frame + 20u);
    value = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kAssignSharedOwnerResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  // Borrow the staging block and pick up the owner's slot entry.
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - kAssignSharedOwnerScratchBytes;
    GuestStackSp(ctx) = scratch;
    StoreU32(ctx, scratch + 12u, shared);
    field_value = LoadU32(ctx, owner + 36u);
    field = (field_value == 0u);
  }

  // Only an owner whose slot entry is empty (or a rewind past this point) has to
  // build the record and fill it in.
  if (field != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Normalize the slot entry: clear the tag word, keep the others live.
    if (ResumePhase(ctx) == kResumeRunning) {
      field_value = owner + 36u;
      field = LoadU32(ctx, owner + 32u);
      StoreU32(ctx, owner + 32u, 0u);
      field = field << 1u;
      StoreU32(ctx, owner + 36u, field);
      if (shared > field) {
        StoreU32(ctx, field_value, shared);
        field = shared;
      }
      shared = LoadU32(ctx, owner + 24u);
      field = (field < shared);
      if (field != 0u) {
        StoreU32(ctx, field_value, shared);
      }
    }

    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      CreateSharedRecord(ctx, scratch, field_value);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      staging = HostFoldLayoutOffset(ctx, 1032u, 16u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      shared = staging;
    }

    // Fill the record in.  A rewind keeps the pointer it already restored.
    if (ResumePhase(ctx) == kResumeRunning) {
      field_value = LoadU32(ctx, kAssignSharedOwnerPendingFlag);
      StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
      field = (field_value == 1u);
      if (field != 0u) {
        goto read_call_result;
      }
      StoreU32(ctx, shared + 8u, LoadU32(ctx, scratch));
      field_value = LoadU32(ctx, scratch + 4u);
      StoreU32(ctx, shared + 4u, owner + 4u);
      StoreU32(ctx, shared + 12u, field_value);
      field_value = LoadU32(ctx, owner + 4u);
      StoreU32(ctx, shared, field_value);
      StoreU32(ctx, field_value + 4u, shared);
      StoreU32(ctx, owner + 4u, shared);
      StoreU32(ctx, owner + 12u, LoadU32(ctx, owner + 12u) + 1u);
      field_value = LoadU32(ctx, owner + 36u);
      shared = LoadU32(ctx, scratch + 12u);
    }
  }

  // Dispatch on how much is left and how big a slot is.
  if (ResumePhase(ctx) == kResumeRunning) {
    field = LoadU32(ctx, LoadU32(ctx, owner + 4u) + 8u);
    if (shared <= field_value) {
      goto copy_remaining;
    }
    if (field != 0u) {
      goto copy_chunk;
    }
    StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    HostFailAssertion(ctx, 857u, kData91399, kData913A1, 734u, kData913D9);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    owner = LoadU32(ctx, kAssignSharedOwnerPendingFlag);
    StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
    owner = (owner != 1u);
    if (owner != 0u) {
      goto trap_path;
    }
    goto rethrow_pending;
  }

  // The call this step made returned a record pointer; release it or keep going
  // with the slot it points at.  owner and value carry the result here.
  read_call_result:;
  if (ResumePhase(ctx) == kResumeRunning) {
    value = HostRet0(ctx);
    HostTempRet0(ctx);
    owner = LoadU32(ctx, scratch + 4u);
    if (owner == 0u) {
      goto release_and_trap;
    }
    shared = LoadU32(ctx, owner + 4u);
    field = shared - 1u;
    StoreU32(ctx, owner + 4u, field);
    if (shared != 1u) {
      goto release_and_trap;
    }
    shared = LoadU32(ctx, LoadU32(ctx, owner) + 8u);
    StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    HostReleaseSlotPair(ctx, shared, owner);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    shared = LoadU32(ctx, kAssignSharedOwnerPendingFlag);
    StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
    shared = (shared != 1u);
  }

  // Release the second slot pair the same way.
  if (shared != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      shared = LoadU32(ctx, owner + 8u);
      field = shared - 1u;
      StoreU32(ctx, owner + 8u, field);
      if (shared != 1u) {
        goto release_and_trap;
      }
      shared = LoadU32(ctx, LoadU32(ctx, owner) + 12u);
      StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      HostReleaseSlotPair(ctx, shared, owner);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      owner = LoadU32(ctx, kAssignSharedOwnerPendingFlag);
      StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
      owner = (owner != 1u);
      if (owner != 0u) {
        goto release_and_trap;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_pending;
  }

  // A full slot: copy one chunk, advance the counters and hand the tail back to
  // this same step.
  copy_chunk:;
  if (ResumePhase(ctx) == kResumeRunning) {
    field = LoadU32(ctx, field + 12u) + LoadU32(ctx, field + 4u) + LoadU32(ctx, owner + 32u);
    CopyBytes(ctx, field, value, field_value);
    StoreU32(ctx, owner + 36u, 0u);
    StoreU32(ctx, owner + 28u, LoadU32(ctx, owner + 28u) + field_value);
    StoreU32(ctx, owner + 32u, LoadU32(ctx, owner + 32u) + field_value);
    shared = LoadU32(ctx, scratch + 12u) - field_value;
    value = value + field_value;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    AssignSharedOwner(ctx, owner, value, shared);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto pop_scratch;
  }

  // Everything left fits in one slot: copy it, then fold the counts back.
  copy_remaining:;
  if ((ResumePhase(ctx) == kResumeRunning ? field == 0u : field_value != 0u) ||
      ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      HostFailAssertion(ctx, 857u, kData91399, kData913A1, 734u, kData913D9);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      owner = LoadU32(ctx, kAssignSharedOwnerPendingFlag);
      StoreU32(ctx, kAssignSharedOwnerPendingFlag, 0u);
      owner = (owner != 1u);
      if (owner != 0u) {
        goto trap_path;
      }
      goto rethrow_pending;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    CopyBytes(ctx,
              LoadU32(ctx, owner + 32u) + LoadU32(ctx, field + 12u) + LoadU32(ctx, field + 4u),
              value, shared);
    value = LoadU32(ctx, scratch + 12u);
    StoreU32(ctx, owner + 28u, LoadU32(ctx, owner + 28u) + value);
    StoreU32(ctx, owner + 32u, LoadU32(ctx, owner + 32u) + value);
    value = LoadU32(ctx, owner + 36u) - value;
    StoreU32(ctx, owner + 36u, value);
  }

  // Done: give the staging block back to the guest stack and leave.
  pop_scratch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + kAssignSharedOwnerScratchBytes;
    goto done;
  }
  trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  release_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, value);
    TrapUnreachable();
  }
  rethrow_pending:;
  if (ResumePhase(ctx) == kResumeRunning) {
    owner = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    Panic(ctx, owner);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto done;

  suspend_point:;
  // Suspend: push the resume point, then the 24-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kAssignSharedOwnerResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, owner);
    StoreU32(ctx, frame + 4u, value);
    StoreU32(ctx, frame + 8u, shared);
    StoreU32(ctx, frame + 12u, field);
    StoreU32(ctx, frame + 16u, field_value);
    StoreU32(ctx, frame + 20u, scratch);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kAssignSharedOwnerSaveFrameBytes);
  done:;
  LeaveFrame();
}
}  // namespace artc
