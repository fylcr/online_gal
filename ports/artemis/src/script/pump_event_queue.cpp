#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the handoff and the dispatch steps this round drives.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t PollEventKind(EngineContext* ctx);
void RunEventStep(EngineContext* ctx, uint32_t a0);
namespace AutoReadTimer {
  void Dispatch(EngineContext* ctx, uint32_t a0, uint32_t a1);
}
namespace BacklogView {
  void DispatchAux(EngineContext* ctx, uint32_t a0, uint32_t a1);
}

namespace {

// The shared_ptr precondition the five checks below report: the leading word,
// the "px != 0" expression, its source file, the line, and the enclosing
// function.
constexpr uint32_t kPumpEventQueueAssertLead = 857u;
constexpr uint32_t kPumpEventQueueNullCheckAddr = 0xBE5BDu;
constexpr uint32_t kPumpEventQueueSharedPtrPathAddr = 0xBE5C5u;
constexpr uint32_t kPumpEventQueueAssertLine = 734u;
constexpr uint32_t kPumpEventQueueAssertFuncAddr = 0xBE5FDu;
// The engine's pending-throw latch, cleared before every bridge call that can
// raise and read back once it returns.
constexpr uint32_t kPumpEventQueueThrowLatch = 0xD015Cu;
// The queue's drain flag: a byte the last step of a round raises once the round
// has handed its work off.
constexpr uint32_t kPumpEventQueueDrainFlagAddr = 0xCF7DCu;
// The queue's head word, sampled again after every dispatched entry.
constexpr uint32_t kPumpEventQueueHeadAddr = 0xCF7D4u;

}  // namespace

// Pumps one round of the pending event queue: samples the drain flag and the
// queue head, then walks the guarded steps below -- each either dispatches the
// entry it holds or hands the round to the catch machinery. A check whose latch
// comes back clear has no exception to hand off and traps. The check groups are
// flat, one after another: a group that finds nothing left to dispatch falls
// through to the next step of the same round. The body is suspendable: a
// rewinding pass restores the two slots below from the frame the suspend path
// pushed, and the resume code names the call to replay.
void PumpEventQueue(EngineContext* ctx) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 8u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the ten spill sites below name.
  uint32_t const kFirstAssertCall = 0u;
  uint32_t const kSecondAssertCall = 1u;
  uint32_t const kDispatchCall = 2u;
  uint32_t const kThirdAssertCall = 3u;
  uint32_t const kBacklogAuxCall = 4u;
  uint32_t const kFourthAssertCall = 5u;
  uint32_t const kAutoReadCall = 6u;
  uint32_t const kFifthAssertCall = 7u;
  uint32_t const kRunEventCall = 8u;
  uint32_t const kHandoffCall = 9u;
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it as a flag.
  uint32_t entry = 0u;
  uint32_t pending = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path pushed and rebuild the
    // two saved slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    entry = LoadU32(ctx, frame);
    pending = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay
    // separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the host a slice, then read the drain flag and the queue head.
    HostYieldToHost(ctx);
    pending = static_cast<uint32_t>(LoadU8(ctx, kPumpEventQueueDrainFlagAddr) == 0u);
    entry = LoadU32(ctx, kPumpEventQueueHeadAddr);
  }
  // A round is pumped only while something is queued, or while a rewinding pass
  // re-walks the steps it already took.
  if (pending || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Nothing is queued when the head word is empty.
      pending = static_cast<uint32_t>(entry == 0u);
    }
    if (pending || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstAssertCall) : 1u) {
        HostFailAssertion(ctx, kPumpEventQueueAssertLead, kPumpEventQueueNullCheckAddr,
                          kPumpEventQueueSharedPtrPathAddr, kPumpEventQueueAssertLine,
                          kPumpEventQueueAssertFuncAddr);
        resume_code = kFirstAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // The latch holds 1 while an exception is in flight.
        entry = LoadU32(ctx, kPumpEventQueueThrowLatch);
        StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
        if (entry != 1u) {
          goto trap_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Step into the entry: an empty follow-up word ends the round.
      entry = LoadU32(ctx, entry + 24u);
      pending = static_cast<uint32_t>(entry == 0u);
    }
    if (pending || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondAssertCall) : 1u) {
        HostFailAssertion(ctx, kPumpEventQueueAssertLead, kPumpEventQueueNullCheckAddr,
                          kPumpEventQueueSharedPtrPathAddr, kPumpEventQueueAssertLine,
                          kPumpEventQueueAssertFuncAddr);
        resume_code = kSecondAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        entry = LoadU32(ctx, kPumpEventQueueThrowLatch);
        StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
        if (entry != 1u) {
          goto trap_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The entry's first word names the table its dispatch slot lives in.
      pending = LoadU32(ctx, LoadU32(ctx, entry) + 8u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDispatchCall) : 1u) {
      // Dispatch the entry through the table slot.
      uint32_t const call_result = CallTableT2(ctx, pending, entry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kDispatchCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      entry = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A dispatched entry ends the round; otherwise sample the head again.
      if (entry) {
        goto round_end;
      }
      entry = LoadU32(ctx, kPumpEventQueueHeadAddr);
      pending = static_cast<uint32_t>(entry == 0u);
    }
    if (pending || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdAssertCall) : 1u) {
        HostFailAssertion(ctx, kPumpEventQueueAssertLead, kPumpEventQueueNullCheckAddr,
                          kPumpEventQueueSharedPtrPathAddr, kPumpEventQueueAssertLine,
                          kPumpEventQueueAssertFuncAddr);
        resume_code = kThirdAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        entry = LoadU32(ctx, kPumpEventQueueThrowLatch);
        StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
        if (entry != 1u) {
          goto trap_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kBacklogAuxCall) : 1u) {
      // Hand the entry to the backlog view's second dispatch unit.
      BacklogView::DispatchAux(ctx, entry, 0u);
      resume_code = kBacklogAuxCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The round ends here: raise the drain flag and leave.
      StoreU8(ctx, kPumpEventQueueDrainFlagAddr, 1u);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    pending = static_cast<uint32_t>(entry == 0u);
  }
  if (pending || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFourthAssertCall) : 1u) {
      HostFailAssertion(ctx, kPumpEventQueueAssertLead, kPumpEventQueueNullCheckAddr,
                        kPumpEventQueueSharedPtrPathAddr, kPumpEventQueueAssertLine,
                        kPumpEventQueueAssertFuncAddr);
      resume_code = kFourthAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      entry = LoadU32(ctx, kPumpEventQueueThrowLatch);
      StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
      if (entry != 1u) {
        goto trap_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    pending = PollEventKind(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAutoReadCall) : 1u) {
    // Tick the auto-read timer against the entry's kind.
    AutoReadTimer::Dispatch(ctx, entry, pending);
    resume_code = kAutoReadCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    entry = LoadU32(ctx, kPumpEventQueueHeadAddr);
    pending = static_cast<uint32_t>(entry == 0u);
  }
  if (pending || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFifthAssertCall) : 1u) {
      HostFailAssertion(ctx, kPumpEventQueueAssertLead, kPumpEventQueueNullCheckAddr,
                        kPumpEventQueueSharedPtrPathAddr, kPumpEventQueueAssertLine,
                        kPumpEventQueueAssertFuncAddr);
      resume_code = kFifthAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      entry = LoadU32(ctx, kPumpEventQueueThrowLatch);
      StoreU32(ctx, kPumpEventQueueThrowLatch, 0u);
      if (entry != 1u) {
        goto trap_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRunEventCall) : 1u) {
    // Run the entry's event step.
    RunEventStep(ctx, entry);
    resume_code = kRunEventCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  round_end:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto done;
  }
  trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    entry = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kHandoffCall) : 1u) {
    Panic(ctx, entry);
    resume_code = kHandoffCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto done;
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 8-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, entry);
    StoreU32(ctx, frame + 4u, pending);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
done:;
  LeaveFrame();
}

}  // namespace artc
