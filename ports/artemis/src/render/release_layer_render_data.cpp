#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the failure handoff.
void Panic(EngineContext* ctx, uint32_t a0);

// Tears down the render data a layer still owns. The row walk visits the record
// slots between the layer's index pair at +572..+576, selects each record out of
// the block table at +492, and gives up the two reference halves of the object
// the record points at (+52). The layer's own cursor slot at +584 is released the
// same way afterwards. Every release goes through the engine's failure bridge, so
// the body is suspendable and replays the call it was suspended in.
void ReleaseLayerRenderData(EngineContext* ctx, uint32_t layer) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 20u;  // the layer and the four slots below
  uint32_t const kResumeWordBytes = 4u;
  // The slot the failure bridge raises its pending-throw flag in.
  uint32_t const kPendingThrowSlot = 0xD015Cu;
  // The cursor value that marks the walk as cleared.
  uint32_t const kInvalidIndex = 0xFFFFFFFFu;
  // The codes the driver spills at the six await sites, naming the call that was
  // in flight.
  uint32_t const kFirstRefRelease = 0u;
  uint32_t const kSecondRefRelease = 1u;
  uint32_t const kCursorDisposeCall = 2u;
  uint32_t const kObjectRefRelease = 3u;
  uint32_t const kObjectSecondRelease = 4u;
  uint32_t const kFailureHandoff = 5u;
  // The walk reuses four slots: one that carries a layer field, the slot ordinal
  // and the object read out of a record; a latch that carries a null or zero test
  // and the row counter; a cursor that carries a record and the field read out of
  // it; and a scratch pointer. All four are declared up here because the labels
  // below jump past their assignment points and because a rewinding pass restores
  // every one of them from the saved frame.
  uint32_t slot = 0u;
  uint32_t latch = 0u;
  uint32_t cursor = 0u;
  uint32_t scratch = 0u;
  uint32_t frame = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path pushed and rebuild the
    // layer, the slot, the cursor, the scratch and the latch from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    layer = LoadU32(ctx, frame);
    slot = LoadU32(ctx, frame + 4u);
    cursor = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    latch = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate — the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the layer's cursor slot at +584 holds the object the tail releases,
    // and a null there is the latch the whole walk is gated on.
    slot = LoadU32(ctx, layer + 584u);
    latch = static_cast<uint32_t>(slot == 0u);
  }
  if (latch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: the walk covers the index pair at +572..+576 and is entered only
      // while the first index is not past the second.
      slot = LoadU32(ctx, layer + 572u);
      latch = static_cast<uint32_t>(static_cast<int32_t>(slot) <=
                                    static_cast<int32_t>(LoadU32(ctx, layer + 576u)));
    }
    if (latch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    row_walk:
      if (ResumePhase(ctx) == kResumeRunning) {
        // Running: the slot ordinal is the layer's base ordinal at +504 plus the
        // index being walked. A block pointer out of the table at +492 names the
        // record: one 60-byte element of the 68 the block holds.
        latch = slot;
        const uint32_t blocks = LoadU32(ctx, layer + 492u);
        slot = LoadU32(ctx, layer + 504u) + slot;
        const uint32_t row = DIV_U(slot, 68u);
        cursor = LoadU32(ctx, blocks + (row << 2u)) + (slot - row * 68u) * 60u;
        // The record's object pointer at +52 is taken and cleared before it is
        // looked at.
        StoreU32(ctx, cursor + 48u, 0u);
        slot = LoadU32(ctx, cursor + 52u);
        StoreU32(ctx, cursor + 52u, 0u);
        latch = static_cast<uint32_t>(slot == 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // Running: the object is given up only when the count at +4 was its last
        // reference; the disposer is the entry the object's own first word names
        // at +8.
        if (latch) {
          goto row_done;
        }
        cursor = LoadU32(ctx, slot + 4u);
        StoreU32(ctx, slot + 4u, cursor - 1u);
        if (cursor != 1u) {
          goto row_done;
        }
        scratch = LoadU32(ctx, slot);
        cursor = LoadU32(ctx, scratch + 8u);
        StoreU32(ctx, kPendingThrowSlot, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstRefRelease) : 1u) {
        HostReleaseSlotPair(ctx, cursor, slot);
        if (ResumePhase(ctx) == kResumeSuspended) {
          resume_code = kFirstRefRelease;
          goto suspend_frame;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // Running: a bridge that came back without raising is not a state the walk
        // continues from; the pending-throw flag decides between the catch path and
        // the object's second reference half.
        cursor = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (cursor == 1u) {
          goto catch_path;
        }
        cursor = LoadU32(ctx, slot + 8u);
        StoreU32(ctx, slot + 8u, cursor - 1u);
        if (cursor != 1u) {
          goto row_done;
        }
        scratch = LoadU32(ctx, slot);
        cursor = LoadU32(ctx, scratch + 12u);
        StoreU32(ctx, kPendingThrowSlot, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondRefRelease) : 1u) {
        HostReleaseSlotPair(ctx, cursor, slot);
        if (ResumePhase(ctx) == kResumeSuspended) {
          resume_code = kSecondRefRelease;
          goto suspend_frame;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto catch_path;
        }
      }
    row_done:
      if (ResumePhase(ctx) == kResumeRunning) {
        // Running: advance to the next slot ordinal and repeat while the layer's
        // last index has not been passed.
        slot = latch + 1u;
        scratch = LoadU32(ctx, layer + 576u);
        latch = static_cast<uint32_t>(static_cast<int32_t>(scratch) > static_cast<int32_t>(latch));
        if (latch) {
          goto row_walk;
        }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: the walk is over. The index pair is given up with one 8-byte store
      // of all ones, so neither the cursor at +572 nor the last index at +576
      // survives it.
      StoreU32(ctx, layer + 580u, 0u);
      StoreU64(ctx, layer + 572u, 0xFFFFFFFFFFFFFFFFull);
      goto exit_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the object the cursor slot at +584 holds is released through the
    // dispose entry its own first word names at +376, and the layer's own storage
    // at +628 is the call's second argument.
    scratch = layer + 628u;
    latch = LoadU32(ctx, LoadU32(ctx, slot) + 376u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCursorDisposeCall) : 1u) {
    CallTableT0(ctx, latch, slot, scratch);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_code = kCursorDisposeCall;
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: clear the cursor slot, then take the object the layer's second
    // cursor at +588 holds and clear that too.
    StoreU32(ctx, layer + 584u, 0u);
    slot = LoadU32(ctx, layer + 588u);
    StoreU32(ctx, layer + 588u, 0u);
    latch = static_cast<uint32_t>(slot == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the same two reference halves, this time on the object the second
    // cursor held.
    if (latch) {
      goto records_done;
    }
    cursor = LoadU32(ctx, slot + 4u);
    StoreU32(ctx, slot + 4u, cursor - 1u);
    if (cursor != 1u) {
      goto records_done;
    }
    cursor = LoadU32(ctx, slot);
    latch = LoadU32(ctx, cursor + 8u);
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kObjectRefRelease) : 1u) {
    HostReleaseSlotPair(ctx, latch, slot);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_code = kObjectRefRelease;
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    latch = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (latch == 1u) {
      goto catch_path;
    }
    cursor = LoadU32(ctx, slot + 8u);
    StoreU32(ctx, slot + 8u, cursor - 1u);
    if (cursor != 1u) {
      goto records_done;
    }
    cursor = LoadU32(ctx, slot);
    latch = LoadU32(ctx, cursor + 12u);
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kObjectSecondRelease) : 1u) {
    HostReleaseSlotPair(ctx, latch, slot);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_code = kObjectSecondRelease;
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (slot == 1u) {
      goto catch_path;
    }
  }
records_done:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the walk is cleared — the cursor at +572 is marked invalid.
    StoreU32(ctx, layer + 572u, kInvalidIndex);
    goto exit_frame;
  }
catch_path:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the failure is in flight. The engine is asked which catch it
    // matches — there is no type filter at this site, so the filter word is zero
    // — and the runner's return slot is read back.
    layer = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFailureHandoff) : 1u) {
    Panic(ctx, layer);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_code = kFailureHandoff;
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_frame;
suspend_frame:
  // Suspended: park the code naming the call that was in flight above the frame,
  // step past it, then write the 20-byte frame a rewinding pass reads back — the
  // layer and the four slots.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, layer);
  StoreU32(ctx, frame + 4u, slot);
  StoreU32(ctx, frame + 8u, latch);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, scratch);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
exit_frame:
  // Every path leaves the frame here, the suspend path falling in from above.
  LeaveFrame();
}

}  // namespace artc
