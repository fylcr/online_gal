#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the failure handoff.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

// The boost precondition the failure bridge reports out of
// shared_ptr::operator->: the leading word, the "px != 0" expression, its source
// file, the line, and the enclosing function.
constexpr uint32_t kReleaseHolderAssertLead = 857u;
constexpr uint32_t kReleaseHolderNullCheckAddr = 0xBCA76u;
constexpr uint32_t kReleaseHolderSharedPtrPathAddr = 0xBCA7Eu;
constexpr uint32_t kReleaseHolderAssertLine = 734u;
constexpr uint32_t kReleaseHolderAssertFuncAddr = 0xBCAB6u;
// The engine's pending-throw latch, cleared before every bridge call that can
// raise and read back once it returns.
constexpr uint32_t kReleaseHolderThrowLatch = 0xD015Cu;
// The all-ones word the object disposer is handed when the call unwinds the whole
// frame.
constexpr uint32_t kReleaseHolderDispatchSentinel = 0xFFFFFFFFu;

}  // namespace

// Drops one reference to a shared object holder and disposes the object behind it
// once the last owner is gone. The body is suspendable: every bridge it calls can
// run guest code, so a rewinding pass restores the slots below from a saved frame
// and replays exactly the one call its resume code names.
void ReleaseSharedObjectHolder(EngineContext* ctx, uint32_t holder) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 20u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the driver spills at the eleven await sites. The first is the null
  // check the body opens with and is tested as a bare zero; every later site
  // numbers itself.
  uint32_t const kNullCheckCall = 0u;
  uint32_t const kScheduleCall = 1u;
  uint32_t const kFirstSlotRelease = 2u;
  uint32_t const kSecondSlotRelease = 3u;
  uint32_t const kCountedNullCheckCall = 4u;
  uint32_t const kThirdSlotRelease = 5u;
  uint32_t const kFourthSlotRelease = 6u;
  uint32_t const kObjectDisposeCall = 7u;
  uint32_t const kTailNullCheckCall = 8u;
  uint32_t const kTailDisposeCall = 9u;
  uint32_t const kPanicHandoff = 10u;
  // The reused slots, all declared up here because a rewinding pass restores
  // every one of them from the saved frame and because the labels below are
  // jumped past their own assignment points: a pair of carriers that stand in
  // turn for an owner pointer, a refcount or a vtable entry, the 16-byte pair
  // this body parks on the guest stack and that pair's address, plus the code
  // naming the call in flight.
  uint32_t slot = 0u;
  uint32_t entry = 0u;
  uint32_t scratch = 0u;
  uint32_t pair_addr = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path pushed and rebuild the
    // holder and the four carriers from it, in the order they were written.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    holder = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
    entry = LoadU32(ctx, saved_frame + 8u);
    scratch = LoadU32(ctx, saved_frame + 12u);
    pair_addr = LoadU32(ctx, saved_frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay
    // separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
    // The holder's step count at +48 says whether there is an object to release
    // at all; the flag is the latch the block below is gated on.
    slot = static_cast<uint32_t>(LoadU32(ctx, holder + 48u) == 2u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The holder's first owner pointer at +32; a null one is the latch the null
      // check below is gated on.
      slot = LoadU32(ctx, holder + 32u);
      entry = static_cast<uint32_t>(slot == 0u);
    }
    if (entry || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNullCheckCall) : 1u) {
        HostFailAssertion(ctx, kReleaseHolderAssertLead, kReleaseHolderNullCheckAddr,
                          kReleaseHolderSharedPtrPathAddr, kReleaseHolderAssertLine,
                          kReleaseHolderAssertFuncAddr);
        resume_code = kNullCheckCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // A bridge that came back without raising is not a state this body
        // continues from: the throw flag decides where the walk goes next.
        holder = LoadU32(ctx, kReleaseHolderThrowLatch);
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
        holder = static_cast<uint32_t>(holder != 1u);
        if (holder) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Blank the pair the scheduler bridge is handed, read the object's vtable
      // entry at +216 and clear the throw latch before the call.
      StoreU64(ctx, scratch + 8u, 0ull);
      entry = LoadU32(ctx, LoadU32(ctx, slot) + 216u);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      pair_addr = scratch + 8u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kScheduleCall) : 1u) {
      HostOsSchedule(ctx, entry, slot, pair_addr);
      resume_code = kScheduleCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kReleaseHolderThrowLatch);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      slot = static_cast<uint32_t>(slot != 1u);
    }
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // The second owner pointer, kept in the scratch pair at +12.
        slot = LoadU32(ctx, scratch + 12u);
        entry = static_cast<uint32_t>(slot == 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // A step down that leaves the count at one still has an owner, so the
        // release call and the disposer read below are both skipped.
        if (entry) {
          goto slot_release_done;
        }
        entry = LoadU32(ctx, slot + 4u);
        StoreU32(ctx, slot + 4u, entry - 1u);
        entry = static_cast<uint32_t>(entry != 1u);
        if (entry) {
          goto slot_release_done;
        }
        entry = LoadU32(ctx, LoadU32(ctx, slot) + 8u);
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstSlotRelease) : 1u) {
        HostReleaseSlotPair(ctx, entry, slot);
        resume_code = kFirstSlotRelease;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        entry = LoadU32(ctx, kReleaseHolderThrowLatch);
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
        if (entry == 1u) {
          goto catch_path;
        }
        entry = LoadU32(ctx, slot + 8u);
        StoreU32(ctx, slot + 8u, entry - 1u);
        entry = static_cast<uint32_t>(entry != 1u);
        if (entry) {
          goto slot_release_done;
        }
        entry = LoadU32(ctx, LoadU32(ctx, slot) + 12u);
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondSlotRelease) : 1u) {
        HostReleaseSlotPair(ctx, entry, slot);
        resume_code = kSecondSlotRelease;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kReleaseHolderThrowLatch);
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto catch_path;
        }
      }
    slot_release_done:;
      if (ResumePhase(ctx) == kResumeRunning) {
        // The holder's own owner pointer at +32 again. When it is set the object
        // is disposed straight away and the counted chain below is skipped.
        slot = LoadU32(ctx, holder + 32u);
        if (slot) {
          goto dispose_holder;
        }
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCountedNullCheckCall) : 1u) {
        HostFailAssertion(ctx, kReleaseHolderAssertLead, kReleaseHolderNullCheckAddr,
                          kReleaseHolderSharedPtrPathAddr, kReleaseHolderAssertLine,
                          kReleaseHolderAssertFuncAddr);
        resume_code = kCountedNullCheckCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        holder = LoadU32(ctx, kReleaseHolderThrowLatch);
        StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
        holder = static_cast<uint32_t>(holder != 1u);
        if (holder) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The runner's return slot carries the object this walk ends on, and the
      // pair at +12 names the object whose counted chain is stepped down next.
      // The holder slot doubles as that carry word from here on.
      slot = HostRet0(ctx);
      (void)HostTempRet0(ctx);
      holder = LoadU32(ctx, scratch + 12u);
      scratch = static_cast<uint32_t>(holder == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A null carry word has no counted chain, so the walk goes straight to the
      // disposer. Otherwise the pair at +4 is stepped down, and only a count that
      // reaches one keeps the walk on this path.
      if (scratch) {
        goto pair_release_done;
      }
      entry = LoadU32(ctx, holder + 4u);
      StoreU32(ctx, holder + 4u, entry - 1u);
      scratch = static_cast<uint32_t>(entry != 1u);
      if (scratch) {
        goto pair_release_done;
      }
      scratch = LoadU32(ctx, LoadU32(ctx, holder) + 8u);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdSlotRelease) : 1u) {
      HostReleaseSlotPair(ctx, scratch, holder);
      resume_code = kThirdSlotRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, kReleaseHolderThrowLatch);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      if (scratch == 1u) {
        goto catch_path;
      }
      entry = LoadU32(ctx, holder + 8u);
      StoreU32(ctx, holder + 8u, entry - 1u);
      scratch = static_cast<uint32_t>(entry != 1u);
      if (scratch) {
        goto pair_release_done;
      }
      scratch = LoadU32(ctx, LoadU32(ctx, holder) + 12u);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFourthSlotRelease) : 1u) {
      HostReleaseSlotPair(ctx, scratch, holder);
      resume_code = kFourthSlotRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      holder = LoadU32(ctx, kReleaseHolderThrowLatch);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      holder = static_cast<uint32_t>(holder == 1u);
      if (holder) {
        goto catch_path;
      }
    }
  pair_release_done:;
    // The counted chain had no last reference of its own: the object is handed to
    // the OS release bridge and the walk is over.
    if (ResumePhase(ctx) == kResumeRunning) {
      HostOsRelease(ctx, slot);
      TrapUnreachable();
    }
  dispose_holder:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The disposer sits at +256 of the object's vtable.
      entry = LoadU32(ctx, LoadU32(ctx, slot) + 256u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kObjectDisposeCall) : 1u) {
      CallTableT5(ctx, entry, slot, kReleaseHolderDispatchSentinel, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kObjectDisposeCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The holder's step byte is raised, its step counter cleared, and the owner
    // pointer at +24 latched -- a null one is the latch below.
    StoreU8(ctx, holder + 60u, 1u);
    StoreU32(ctx, holder + 48u, 0u);
    slot = LoadU32(ctx, holder + 24u);
    entry = static_cast<uint32_t>(slot == 0u);
  }
  if (entry || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTailNullCheckCall) : 1u) {
      HostFailAssertion(ctx, kReleaseHolderAssertLead, kReleaseHolderNullCheckAddr,
                        kReleaseHolderSharedPtrPathAddr, kReleaseHolderAssertLine,
                        kReleaseHolderAssertFuncAddr);
      resume_code = kTailNullCheckCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      holder = LoadU32(ctx, kReleaseHolderThrowLatch);
      StoreU32(ctx, kReleaseHolderThrowLatch, 0u);
      holder = static_cast<uint32_t>(holder != 1u);
      if (holder) {
        goto no_throw_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The tail disposer at +348 of the object's vtable.
    entry = LoadU32(ctx, LoadU32(ctx, slot) + 348u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTailDisposeCall) : 1u) {
    CallTableT1(ctx, entry, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kTailDisposeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The object's flag at +128 is raised to 2 and the scratch pair handed back to
    // the guest stack.
    if (LoadU32(ctx, holder + 128u) != 0u) {
      StoreU32(ctx, holder + 128u, 2u);
    }
    GuestStackSp(ctx) = scratch + 16u;
    goto exit_path;
  }
no_throw_path:;
  // Reached only when a bridge came back without raising, which the walk has no
  // state to continue from.
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The exception is in flight. Ask the engine which catch it matches -- there
    // is no type filter at this site, so the filter word is zero -- and let the
    // bridge read the runner's return slot.
    holder = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPanicHandoff) : 1u) {
    Panic(ctx, holder);
    resume_code = kPanicHandoff;
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
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 20-byte frame a rewinding pass reads back -- the holder and the four
    // carriers, in that order. The stack pointer lives in the word the frame
    // pointer itself names, so every push is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, holder);
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreU32(ctx, saved_frame + 8u, entry);
    StoreU32(ctx, saved_frame + 12u, scratch);
    StoreU32(ctx, saved_frame + 16u, pair_addr);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
