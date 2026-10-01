#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the failure handoff.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

// The shared_ptr precondition this step reports when it finds an owner that has
// already gone: the leading word, the "px != 0" expression, its source file, the
// line, and the enclosing function.
constexpr uint32_t kAdvanceEffectTimerAssertLead = 857u;
constexpr uint32_t kAdvanceEffectTimerNullCheckAddr = 0xBA361u;
constexpr uint32_t kAdvanceEffectTimerSharedPtrPathAddr = 0xBA369u;
constexpr uint32_t kAdvanceEffectTimerAssertLine = 734u;
constexpr uint32_t kAdvanceEffectTimerAssertFuncAddr = 0xBA3A1u;
// The engine's pending-throw latch, cleared before every bridge call that can
// raise and read back once it returns.
constexpr uint32_t kAdvanceEffectTimerThrowLatch = 0xD015Cu;

}  // namespace

// Advances the message effect timer: the pending effect step is timed and applied
// once its interval has elapsed. The body is suspendable -- a rewinding pass
// restores the seven slots below from the frame it pushed and replays the one
// call its resume code names.
uint32_t AdvanceMessageEffectTimer(EngineContext* ctx, uint32_t object, uint32_t carry, uint32_t holder) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 28u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the eighteen call sites below spill.
  uint32_t const kReleaseSlotCall = 0u;
  uint32_t const kFirstInitResourcesCall = 1u;
  uint32_t const kFirstPairRelease = 2u;
  uint32_t const kSecondPairRelease = 3u;
  uint32_t const kFirstAssertCall = 4u;
  uint32_t const kThirdPairRelease = 5u;
  uint32_t const kFourthPairRelease = 6u;
  uint32_t const kAdvanceStepCall = 7u;
  uint32_t const kSecondInitResourcesCall = 8u;
  uint32_t const kFifthPairRelease = 9u;
  uint32_t const kSixthPairRelease = 10u;
  uint32_t const kSecondAssertCall = 11u;
  uint32_t const kFirstStepCall = 12u;
  uint32_t const kSeventhPairRelease = 13u;
  uint32_t const kEighthPairRelease = 14u;
  uint32_t const kSecondStepCall = 15u;
  uint32_t const kFirstHandoffCall = 16u;
  uint32_t const kSecondHandoffCall = 17u;
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it as a carry:
  // object, carry and holder are the call's own arguments, and resume_code,
  // result, pair_addr, target, stack_slot and no_owner are the walk's.
  uint32_t resume_code = 0u;
  uint32_t result = 0u;
  uint32_t pair_addr = 0u;
  uint32_t target = 0u;
  uint32_t stack_slot = 0u;
  uint32_t no_owner = 0u;
  // Plain temporaries, rebuilt from scratch on every pass.
  uint32_t quotient = 0u;
  uint32_t bucket = 0u;
  uint32_t bucket_base = 0u;
  uint32_t node_base = 0u;
  uint32_t slot_base = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path pushed and rebuild the
    // seven saved slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    object = LoadU32(ctx, frame);
    carry = LoadU32(ctx, frame + 4u);
    holder = LoadU32(ctx, frame + 8u);
    result = LoadU32(ctx, frame + 12u);
    target = LoadU32(ctx, frame + 16u);
    stack_slot = LoadU32(ctx, frame + 20u);
    pair_addr = LoadU32(ctx, frame + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay
    // separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 16-byte guest-stack slot holds the two-owner record the calls below
    // take by address; no_owner remembers whether there is an owner at all.
    stack_slot = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = stack_slot;
    target = LoadU32(ctx, object + 584u);
    no_owner = static_cast<uint32_t>(target == 0u);
    result = 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning && no_owner) {
    goto finish;
  }
  if (carry || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The pending step has not started yet: there is nothing to unwind.
      if (LoadU32(ctx, object + 572u) == 0u) {
        goto wrap_up;
      }
      holder = LoadU32(ctx, LoadU32(ctx, target) + 376u);
      carry = object + 604u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReleaseSlotCall) : 1u) {
      CallTableT0(ctx, holder, target, carry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kReleaseSlotCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Step the depth count down, then take the owner pair off the pending
      // record and raise its first refcount.
      carry = LoadU32(ctx, object + 572u) - 1u;
      StoreU32(ctx, object + 572u, carry);
      StoreU32(ctx, stack_slot + 8u, LoadU32(ctx, object + 584u));
      holder = LoadU32(ctx, object + 588u);
      StoreU32(ctx, stack_slot + 12u, holder);
      if (holder) {
        StoreU32(ctx, holder + 4u, LoadU32(ctx, holder + 4u) + 1u);
        carry = LoadU32(ctx, object + 572u);
      }
      // Two hash lookups on the depth count: a node table of 24-byte entries and
      // a slot table of 12-byte entries, each wrapped to its bucket count.
      result = LoadU32(ctx, object + 528u) + carry;
      quotient = DIV_U(result, 170u);
      node_base = LoadU32(ctx, LoadU32(ctx, object + 516u) + (quotient << 2));
      carry = LoadU32(ctx, object + 552u) + carry;
      bucket = DIV_U(carry, 341u);
      slot_base = LoadU32(ctx, LoadU32(ctx, object + 540u) + (bucket << 2));
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      pair_addr = stack_slot + 8u;
      result = node_base + ((result - quotient * 170u) * 24u);
      bucket_base = bucket * 341u;
      carry = slot_base + ((carry - bucket_base) * 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstInitResourcesCall) : 1u) {
      HostInitResources(ctx, 3231u, object, pair_addr, result, carry, 0u);
      resume_code = kFirstInitResourcesCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      carry = static_cast<uint32_t>(carry != 1u);
    }
    if (carry || ResumePhase(ctx) == kResumeRewinding) {
      carry = ResumePhase(ctx) ? carry : static_cast<uint32_t>(holder == 0u);
      if (ResumePhase(ctx) == kResumeRunning) {
        // A pair with no owner, or a count that does not reach one, keeps the
        // pair release below from running at all.
        if (carry) {
          goto slot_release_done;
        }
        carry = LoadU32(ctx, holder + 4u);
        result = carry - 1u;
        StoreU32(ctx, holder + 4u, result);
        carry = static_cast<uint32_t>(carry != 1u);
        if (carry) {
          goto slot_release_done;
        }
        carry = LoadU32(ctx, LoadU32(ctx, holder) + 8u);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstPairRelease) : 1u) {
        HostReleaseSlotPair(ctx, carry, holder);
        resume_code = kFirstPairRelease;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
        if (carry == 1u) {
          goto catch_path;
        }
        carry = LoadU32(ctx, holder + 8u);
        result = carry - 1u;
        StoreU32(ctx, holder + 8u, result);
        carry = static_cast<uint32_t>(carry != 1u);
        if (carry) {
          goto slot_release_done;
        }
        carry = LoadU32(ctx, LoadU32(ctx, holder) + 12u);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondPairRelease) : 1u) {
        HostReleaseSlotPair(ctx, carry, holder);
        resume_code = kSecondPairRelease;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
        carry = static_cast<uint32_t>(carry == 1u);
        if (carry) {
          goto catch_path;
        }
      }
    slot_release_done:;
      if (ResumePhase(ctx) == kResumeRunning) {
        // An owner is on the record: the step is run against it. Otherwise the
        // null check below has to hold.
        carry = LoadU32(ctx, object + 584u);
        if (carry) {
          goto owner_call_path;
        }
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstAssertCall) : 1u) {
        HostFailAssertion(ctx, kAdvanceEffectTimerAssertLead, kAdvanceEffectTimerNullCheckAddr,
                          kAdvanceEffectTimerSharedPtrPathAddr, kAdvanceEffectTimerAssertLine,
                          kAdvanceEffectTimerAssertFuncAddr);
        resume_code = kFirstAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        object = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
        object = static_cast<uint32_t>(object != 1u);
        if (object) {
          goto no_throw_path;
        }
        goto handoff_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The step's own answer is in the runner's return slot; the owner pair it
      // left behind is unwound here.
      object = HostRet0(ctx);
      (void)HostTempRet0(ctx);
      if (holder == 0u) {
        goto os_release_path;
      }
      carry = LoadU32(ctx, holder + 4u);
      result = carry - 1u;
      StoreU32(ctx, holder + 4u, result);
      if (carry != 1u) {
        goto os_release_path;
      }
      carry = LoadU32(ctx, LoadU32(ctx, holder) + 8u);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, carry, holder);
      resume_code = kThirdPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      carry = static_cast<uint32_t>(carry != 1u);
    }
    if (carry || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        carry = LoadU32(ctx, holder + 8u);
        result = carry - 1u;
        StoreU32(ctx, holder + 8u, result);
        carry = static_cast<uint32_t>(carry != 1u);
        if (carry) {
          goto os_release_path;
        }
        carry = LoadU32(ctx, LoadU32(ctx, holder) + 12u);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFourthPairRelease) : 1u) {
        HostReleaseSlotPair(ctx, carry, holder);
        resume_code = kFourthPairRelease;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
        carry = static_cast<uint32_t>(carry != 1u);
        if (carry) {
          goto os_release_path;
        }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto handoff_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // No unwind in flight: time the pending step and apply it. result answers 0
    // when there is no owner to step, 1 once there is.
    result = 0u;
    if (holder == 0u) {
      goto finish;
    }
    result = 1u;
    if (LoadU32(ctx, object + 572u) == LoadU32(ctx, object + 556u) - 1u) {
      goto finish;
    }
    holder = LoadU32(ctx, LoadU32(ctx, target) + 376u);
    carry = object + 628u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAdvanceStepCall) : 1u) {
    CallTableT0(ctx, holder, target, carry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAdvanceStepCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Same bookkeeping as above, on the other owner pair, plus the two hash
    // lookups that name the entry this step is queued against.
    StoreU32(ctx, object + 572u, LoadU32(ctx, object + 572u) + 1u);
    StoreU32(ctx, stack_slot, LoadU32(ctx, object + 584u));
    holder = LoadU32(ctx, object + 588u);
    StoreU32(ctx, stack_slot + 4u, holder);
    if (holder) {
      StoreU32(ctx, holder + 4u, LoadU32(ctx, holder + 4u) + 1u);
      carry = LoadU32(ctx, object + 572u);
    }
    result = LoadU32(ctx, object + 528u) + carry;
    quotient = DIV_U(result, 170u);
    node_base = LoadU32(ctx, LoadU32(ctx, object + 516u) + (quotient << 2));
    carry = LoadU32(ctx, object + 552u) + carry;
    bucket = DIV_U(carry, 341u);
    slot_base = LoadU32(ctx, LoadU32(ctx, object + 540u) + (bucket << 2));
    StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    result = node_base + ((result - quotient * 170u) * 24u);
    bucket_base = bucket * 341u;
    carry = slot_base + ((carry - bucket_base) * 12u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondInitResourcesCall) : 1u) {
    HostInitResources(ctx, 3231u, object, stack_slot, result, carry, 0u);
    resume_code = kSecondInitResourcesCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
    StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    carry = static_cast<uint32_t>(carry != 1u);
  }
  if (carry || ResumePhase(ctx) == kResumeRewinding) {
    carry = ResumePhase(ctx) ? carry : static_cast<uint32_t>(holder == 0u);
    if (ResumePhase(ctx) == kResumeRunning) {
      if (carry) {
        goto pair_release_done;
      }
      carry = LoadU32(ctx, holder + 4u);
      result = carry - 1u;
      StoreU32(ctx, holder + 4u, result);
      carry = static_cast<uint32_t>(carry != 1u);
      if (carry) {
        goto pair_release_done;
      }
      carry = LoadU32(ctx, LoadU32(ctx, holder) + 8u);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFifthPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, carry, holder);
      resume_code = kFifthPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      if (carry == 1u) {
        goto handoff_path;
      }
      carry = LoadU32(ctx, holder + 8u);
      result = carry - 1u;
      StoreU32(ctx, holder + 8u, result);
      carry = static_cast<uint32_t>(carry != 1u);
      if (carry) {
        goto pair_release_done;
      }
      carry = LoadU32(ctx, LoadU32(ctx, holder) + 12u);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSixthPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, carry, holder);
      resume_code = kSixthPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      carry = static_cast<uint32_t>(carry == 1u);
      if (carry) {
        goto handoff_path;
      }
    }
  pair_release_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The owner that would run the second half of the step is looked up here;
      // a missing one has to trip the null check below.
      carry = LoadU32(ctx, object + 584u);
      holder = static_cast<uint32_t>(carry == 0u);
    }
    if (holder || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondAssertCall) : 1u) {
        HostFailAssertion(ctx, kAdvanceEffectTimerAssertLead, kAdvanceEffectTimerNullCheckAddr,
                          kAdvanceEffectTimerSharedPtrPathAddr, kAdvanceEffectTimerAssertLine,
                          kAdvanceEffectTimerAssertFuncAddr);
        resume_code = kSecondAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        object = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
        StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
        object = static_cast<uint32_t>(object != 1u);
        if (object) {
          goto no_throw_path;
        }
        goto handoff_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Run the step's tail against the owner.
      holder = LoadU32(ctx, LoadU32(ctx, carry) + 380u);
      object = object + 616u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstStepCall) : 1u) {
      CallTableT5(ctx, holder, carry, object, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kFirstStepCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto wrap_up;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The tail call's answer is in the runner's return slot; unwind the owner
    // pair it left behind the same way as above.
    object = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    if (holder == 0u) {
      goto os_release_path;
    }
    carry = LoadU32(ctx, holder + 4u);
    result = carry - 1u;
    StoreU32(ctx, holder + 4u, result);
    if (carry != 1u) {
      goto os_release_path;
    }
    carry = LoadU32(ctx, LoadU32(ctx, holder) + 8u);
    StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSeventhPairRelease) : 1u) {
    HostReleaseSlotPair(ctx, carry, holder);
    resume_code = kSeventhPairRelease;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
    StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    carry = static_cast<uint32_t>(carry != 1u);
  }
  if (carry || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, holder + 8u);
      result = carry - 1u;
      StoreU32(ctx, holder + 8u, result);
      carry = static_cast<uint32_t>(carry != 1u);
      if (carry) {
        goto os_release_path;
      }
      carry = LoadU32(ctx, LoadU32(ctx, holder) + 12u);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEighthPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, carry, holder);
      resume_code = kEighthPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, kAdvanceEffectTimerThrowLatch);
      StoreU32(ctx, kAdvanceEffectTimerThrowLatch, 0u);
      carry = static_cast<uint32_t>(carry != 1u);
      if (carry) {
        goto os_release_path;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto handoff_path;
  }
owner_call_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The step's tail, run against the owner found above instead.
    holder = LoadU32(ctx, LoadU32(ctx, carry) + 380u);
    object = object + 592u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondStepCall) : 1u) {
    CallTableT5(ctx, holder, carry, object, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kSecondStepCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
wrap_up:;
  // On the running pass the walk answers 0; a rewinding pass keeps the answer
  // the frame restored.
  result = ResumePhase(ctx) ? result : 0u;
finish:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = stack_slot + 16u;
    goto exit_path;
  }
os_release_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The last reference is gone: hand the object to the OS release bridge.
    HostOsRelease(ctx, object);
    TrapUnreachable();
  }
catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The pair release raised. Ask the engine which catch the in-flight
    // exception matches -- there is no type filter at this site -- and let the
    // bridge read the runner's return slot.
    object = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstHandoffCall) : 1u) {
    Panic(ctx, object);
    resume_code = kFirstHandoffCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
no_throw_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
handoff_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    object = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondHandoffCall) : 1u) {
    Panic(ctx, object);
    resume_code = kSecondHandoffCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  TrapUnreachable();
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 28-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, object);
    StoreU32(ctx, frame + 4u, carry);
    StoreU32(ctx, frame + 8u, holder);
    StoreU32(ctx, frame + 12u, result);
    StoreU32(ctx, frame + 16u, target);
    StoreU32(ctx, frame + 20u, stack_slot);
    StoreU32(ctx, frame + 24u, pair_addr);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
  result = 0u;
exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
