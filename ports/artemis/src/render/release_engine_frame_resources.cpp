#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the failure handoff.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

// The boost precondition the failure bridge reports out of
// shared_ptr::operator->: the leading word, the "px != 0" expression, its source
// file, the line, and the enclosing function.
constexpr uint32_t kFrameReleaseAssertLead = 857u;
constexpr uint32_t kFrameReleaseNullCheckAddr = 0xBCA76u;
constexpr uint32_t kFrameReleaseSharedPtrPathAddr = 0xBCA7Eu;
constexpr uint32_t kFrameReleaseAssertLine = 734u;
constexpr uint32_t kFrameReleaseAssertFuncAddr = 0xBCAB6u;
// The engine's pending-throw latch, cleared before every bridge call that can
// raise and read back once it returns.
constexpr uint32_t kFrameReleaseThrowLatch = 0xD015Cu;

}  // namespace

// Releases what an engine frame still holds when the frame is torn down: the pool
// its +24 word points at, the entry list running from +108 to the past-the-end
// marker at +112, and the second list running from +76 to its marker at +80. The
// pool and every entry are given back through a virtual release call followed by a
// virtual teardown call, each behind the null check of the pointer it goes through.
// The walk is suspendable: every bridge call can run guest code, so a rewinding
// pass restores the slots below from a saved frame and replays exactly the one call
// its resume code names.
void ReleaseEngineFrameResources(EngineContext* ctx, uint32_t frame) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 24u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the driver spills at the thirteen await sites below, naming the call
  // that was in flight: the null check ahead of each of the three release calls and
  // ahead of each of the three teardown calls, the six calls themselves, and the
  // handoff that ends a failed walk.
  uint32_t const kPoolReleaseCheck = 0u;
  uint32_t const kPoolReleaseCall = 1u;
  uint32_t const kPoolTeardownCheck = 2u;
  uint32_t const kPoolTeardownCall = 3u;
  uint32_t const kEntryReleaseCheck = 4u;
  uint32_t const kEntryReleaseCall = 5u;
  uint32_t const kEntryTeardownCheck = 6u;
  uint32_t const kEntryTeardownCall = 7u;
  uint32_t const kViewReleaseCheck = 8u;
  uint32_t const kViewReleaseCall = 9u;
  uint32_t const kViewTeardownCheck = 10u;
  uint32_t const kViewTeardownCall = 11u;
  uint32_t const kPanicHandoff = 12u;
  // The reused slots, all declared up here because a rewinding pass restores every
  // one of them from the saved frame and because the labels below are jumped past
  // their own assignment points: a carrier that stands in turn for a virtual call
  // target, an argument or a comparison result, the entry being visited, a scan
  // cursor, the argument the frame's calls carry and the past-the-end marker --
  // plus the code naming the call in flight.
  uint32_t slot = 0u;
  uint32_t entry = 0u;
  uint32_t cursor = 0u;
  uint32_t call_args = 0u;
  uint32_t sentinel = 0u;
  uint32_t resume_code = 0u;
  float weight = 1.0f;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame the suspend path pushed and rebuild the
    // frame and the five carriers from it, in the order they were written.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    frame = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
    entry = LoadU32(ctx, saved_frame + 8u);
    cursor = LoadU32(ctx, saved_frame + 12u);
    call_args = LoadU32(ctx, saved_frame + 16u);
    sentinel = LoadU32(ctx, saved_frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay
    // separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The frame's word at +48 says there is nothing to release. Otherwise the pool
    // pointer at +24 is the first thing the release goes through, and a null one is
    // the latch the check below is gated on.
    if (LoadU32(ctx, frame + 48u) != 0u) {
      goto release_done;
    }
    entry = LoadU32(ctx, frame + 24u);
    slot = static_cast<uint32_t>(entry == 0u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPoolReleaseCheck) : 1u) {
      HostFailAssertion(ctx, kFrameReleaseAssertLead, kFrameReleaseNullCheckAddr,
                        kFrameReleaseSharedPtrPathAddr, kFrameReleaseAssertLine,
                        kFrameReleaseAssertFuncAddr);
      resume_code = kPoolReleaseCheck;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A bridge that came back without raising is not a state this body continues
      // from: the throw latch decides where the walk goes next.
      frame = LoadU32(ctx, kFrameReleaseThrowLatch);
      StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
      frame = static_cast<uint32_t>(frame != 1u);
      if (frame) {
        goto no_throw_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The pool's release entry is the vtable word at +12, called with the argument
    // the frame carries at +196 and a 1.0 weight.
    call_args = frame + 196u;
    slot = LoadU32(ctx, LoadU32(ctx, entry) + 12u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPoolReleaseCall) : 1u) {
    CallTableT13(ctx, slot, entry, call_args, weight);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kPoolReleaseCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The same null check on the pool, this time ahead of its teardown entry.
    entry = LoadU32(ctx, frame + 24u);
    slot = static_cast<uint32_t>(entry == 0u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPoolTeardownCheck) : 1u) {
      HostFailAssertion(ctx, kFrameReleaseAssertLead, kFrameReleaseNullCheckAddr,
                        kFrameReleaseSharedPtrPathAddr, kFrameReleaseAssertLine,
                        kFrameReleaseAssertFuncAddr);
      resume_code = kPoolTeardownCheck;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      frame = LoadU32(ctx, kFrameReleaseThrowLatch);
      StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
      frame = static_cast<uint32_t>(frame != 1u);
      if (frame) {
        goto no_throw_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The pool's teardown entry is the vtable word at +140 and takes no further
    // argument.
    slot = LoadU32(ctx, LoadU32(ctx, entry) + 140u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPoolTeardownCall) : 1u) {
    CallTableT1(ctx, slot, entry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kPoolTeardownCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The frame's entry list runs from +108 to the past-the-end marker at +112; the
    // walk only opens while the two differ.
    entry = LoadU32(ctx, frame + 108u);
    sentinel = frame + 112u;
    slot = static_cast<uint32_t>(entry != sentinel);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
  pool_walk:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Hold on to the entry being visited, then step to the next one through its
      // word at +28; a null there is the latch the check below is gated on.
      cursor = entry;
      entry = LoadU32(ctx, entry + 28u);
      slot = static_cast<uint32_t>(entry == 0u);
    }
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEntryReleaseCheck) : 1u) {
        HostFailAssertion(ctx, kFrameReleaseAssertLead, kFrameReleaseNullCheckAddr,
                          kFrameReleaseSharedPtrPathAddr, kFrameReleaseAssertLine,
                          kFrameReleaseAssertFuncAddr);
        resume_code = kEntryReleaseCheck;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        frame = LoadU32(ctx, kFrameReleaseThrowLatch);
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
        frame = static_cast<uint32_t>(frame != 1u);
        if (frame) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The entry's release entry, called with the frame argument and 1.0.
      slot = LoadU32(ctx, LoadU32(ctx, entry) + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEntryReleaseCall) : 1u) {
      CallTableT13(ctx, slot, entry, call_args, weight);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kEntryReleaseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The entry's own word at +28 again, this time ahead of its teardown.
      entry = LoadU32(ctx, cursor + 28u);
      slot = static_cast<uint32_t>(entry == 0u);
    }
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEntryTeardownCheck) : 1u) {
        HostFailAssertion(ctx, kFrameReleaseAssertLead, kFrameReleaseNullCheckAddr,
                          kFrameReleaseSharedPtrPathAddr, kFrameReleaseAssertLine,
                          kFrameReleaseAssertFuncAddr);
        resume_code = kEntryTeardownCheck;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        frame = LoadU32(ctx, kFrameReleaseThrowLatch);
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
        frame = static_cast<uint32_t>(frame != 1u);
        if (frame) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, LoadU32(ctx, entry) + 140u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEntryTeardownCall) : 1u) {
      CallTableT1(ctx, slot, entry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kEntryTeardownCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Unlink the entry the walk held on to. An entry without a child chain is
      // taken out through the link word that points back at it; one with a chain
      // hands the chain's last element to the comparison below.
      slot = LoadU32(ctx, cursor + 4u);
      if (slot == 0u) {
        entry = LoadU32(ctx, cursor + 8u);
        if (cursor != LoadU32(ctx, entry)) {
          slot = cursor + 8u;
          do {
            cursor = LoadU32(ctx, slot);
            slot = cursor + 8u;
            entry = LoadU32(ctx, cursor + 8u);
          } while (LoadU32(ctx, entry) != cursor);
        }
      } else {
        do {
          entry = slot;
          slot = LoadU32(ctx, slot);
        } while (slot);
      }
      if (entry != sentinel) {
        goto pool_walk;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The frame's second list runs from +76 to the past-the-end marker at +80. From
    // here on the parameter slot carries the node being visited.
    entry = LoadU32(ctx, frame + 76u);
    cursor = frame + 80u;
    if (entry == cursor) {
      goto release_done;
    }
  }
view_walk:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The node's flag byte at +36 is the latch its check below is gated on.
    frame = entry;
    entry = static_cast<uint32_t>(LoadU8(ctx, frame + 36u) == 0u);
  }
  if (entry || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Step to the node's successor through its word at +28; a null one leaves the
      // check below with nothing to go through.
      entry = LoadU32(ctx, frame + 28u);
      slot = static_cast<uint32_t>(entry == 0u);
    }
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kViewReleaseCheck) : 1u) {
        HostFailAssertion(ctx, kFrameReleaseAssertLead, kFrameReleaseNullCheckAddr,
                          kFrameReleaseSharedPtrPathAddr, kFrameReleaseAssertLine,
                          kFrameReleaseAssertFuncAddr);
        resume_code = kViewReleaseCheck;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        frame = LoadU32(ctx, kFrameReleaseThrowLatch);
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
        frame = static_cast<uint32_t>(frame != 1u);
        if (frame) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The successor's word at +4 names an object whose release entry at +12 is
      // called with that object as its first argument.
      slot = entry + 4u;
      entry = LoadU32(ctx, LoadU32(ctx, entry + 4u) + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kViewReleaseCall) : 1u) {
      CallTableT13(ctx, entry, slot, call_args, weight);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kViewReleaseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The node's own word at +28, ahead of the teardown of the object it names.
      entry = LoadU32(ctx, frame + 28u);
      slot = static_cast<uint32_t>(entry == 0u);
    }
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kViewTeardownCheck) : 1u) {
        HostFailAssertion(ctx, kFrameReleaseAssertLead, kFrameReleaseNullCheckAddr,
                          kFrameReleaseSharedPtrPathAddr, kFrameReleaseAssertLine,
                          kFrameReleaseAssertFuncAddr);
        resume_code = kViewTeardownCheck;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        frame = LoadU32(ctx, kFrameReleaseThrowLatch);
        StoreU32(ctx, kFrameReleaseThrowLatch, 0u);
        frame = static_cast<uint32_t>(frame != 1u);
        if (frame) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, LoadU32(ctx, entry) + 140u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kViewTeardownCall) : 1u) {
      CallTableT1(ctx, slot, entry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kViewTeardownCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The same unlink as above, this time on the node the parameter slot carries.
    slot = LoadU32(ctx, frame + 4u);
    if (slot == 0u) {
      entry = LoadU32(ctx, frame + 8u);
      if (frame != LoadU32(ctx, entry)) {
        slot = frame + 8u;
        do {
          frame = LoadU32(ctx, slot);
          slot = frame + 8u;
          entry = LoadU32(ctx, frame + 8u);
        } while (LoadU32(ctx, entry) != frame);
      }
    } else {
      do {
        entry = slot;
        slot = LoadU32(ctx, slot);
      } while (slot);
    }
    if (entry != cursor) {
      goto view_walk;
    }
  }
release_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
no_throw_path:;
  // Reached when a bridge came back without raising, and by a rewinding pass that
  // has replayed its call: there is no state the walk continues from.
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The exception is in flight. Ask the engine which catch it matches -- there is
    // no type filter at this site, so the filter word is zero -- and let the bridge
    // read the runner's return slot.
    frame = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPanicHandoff) : 1u) {
    Panic(ctx, frame);
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
    // Suspend: push the code naming the call in flight, step past it, then push the
    // 24-byte frame a rewinding pass reads back -- the frame and the five carriers,
    // in that order. The stack pointer lives in the word the frame pointer itself
    // names, so every push is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, frame);
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreU32(ctx, saved_frame + 8u, entry);
    StoreU32(ctx, saved_frame + 12u, cursor);
    StoreU32(ctx, saved_frame + 16u, call_args);
    StoreU32(ctx, saved_frame + 20u, sentinel);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
