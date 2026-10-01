#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the diagnostic, the line formatter and the storage
// initialiser this reset drives.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void InitNodeStorage(EngineContext* ctx, uint32_t a);

namespace {

// The shared_ptr precondition the checks below report, by message index.
constexpr uint32_t kResetFilterStateAssertLead = 857u;
// "px != 0", by message index.
constexpr uint32_t kResetFilterStateNullCheckAddr = 0xB5E31u;
// "../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp", by message index.
constexpr uint32_t kResetFilterStateSharedPtrPathAddr = 0xB5E39u;
// The source line the checks below report.
constexpr uint32_t kResetFilterStateAssertLine = 734u;
// "operator->", by message index.
constexpr uint32_t kResetFilterStateAssertFuncAddr = 0xB5E71u;
// The engine's pending-throw latch, cleared before every bridge call that can
// raise and read back once it returns.
constexpr uint32_t kResetFilterStateThrowLatch = 0xD015Cu;

}  // namespace

// Resets the event filter state: the latched state is replaced, the filter's
// node is dispatched through its vtable, and the filters that survived the reset
// are re-emitted one line at a time.  Suspendable: every bridge call below can
// suspend the step.
void ResetEventFilterState(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 40u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the eight spill sites below name.
  uint32_t const kFirstAssertCall = 0u;
  uint32_t const kFirstDispatchCall = 1u;
  uint32_t const kSecondAssertCall = 2u;
  uint32_t const kSecondDispatchCall = 3u;
  uint32_t const kInitNodeStorageCall = 4u;
  uint32_t const kFirstFormatLineCall = 5u;
  uint32_t const kSecondFormatLineCall = 6u;
  uint32_t const kPanicCall = 7u;
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it as a carry:
  // `target` holds the dispatch slot, then a byte test, then the walk's slot
  // pointer; `element` holds an element of the walk and then the line's object.
  uint32_t target = 0u;
  uint32_t entry = 0u;
  uint32_t element = 0u;
  uint32_t end_slot = 0u;
  uint32_t count = 0u;
  uint32_t begin_slot = 0u;
  uint32_t self = 0u;
  uint32_t walking = 0u;
  uint32_t state_bits = 0u;
  uint32_t resume_code = 0u;
  // Plain temporaries, rebuilt from scratch on every pass.
  uint32_t selected = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 40-byte frame and rebuild the ten saved slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    target = LoadU32(ctx, frame + 4u);
    element = LoadU32(ctx, frame + 12u);
    end_slot = LoadU32(ctx, frame + 16u);
    count = LoadU32(ctx, frame + 20u);
    begin_slot = LoadU32(ctx, frame + 24u);
    self = LoadU32(ctx, frame + 28u);
    walking = LoadU32(ctx, frame + 32u);
    state_bits = LoadU32(ctx, frame + 36u);
    entry = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight; the two pops stay
    // separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Publish the new state over the old one and keep whether it differs from
    // the cleared value; the filter list base hangs off the object.
    uint32_t const state_addr = a + 2876u;
    state_bits = LoadU32(ctx, state_addr);
    uint32_t const new_state = LoadU32(ctx, a + 2880u);
    StoreU32(ctx, state_addr, new_state);
    target = static_cast<uint32_t>(new_state != 1u);
    entry = LoadU32(ctx, a + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Only a state that just became 1 with the latch byte set reaches the check
    // below; anything else goes straight to the second dispatch.
    if (target != 0u) {
      goto second_dispatch;
    }
    target = static_cast<uint32_t>(LoadU8(ctx, a + 34u) == 0u);
    if (target != 0u) {
      goto second_dispatch;
    }
    target = static_cast<uint32_t>(entry == 0u);
  }
  if (target != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The check reports through the pending-throw latch, so it is cleared
      // first.
      StoreU32(ctx, kResetFilterStateThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstAssertCall) : 1u) {
      // The list has to hold a live shared_ptr before it is walked.
      HostFailAssertion(ctx, kResetFilterStateAssertLead, kResetFilterStateNullCheckAddr,
                        kResetFilterStateSharedPtrPathAddr, kResetFilterStateAssertLine,
                        kResetFilterStateAssertFuncAddr);
      resume_code = kFirstAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A raised throw leaves through the catch path, anything else through the
      // unreachable trap; both sit past the walk below.
      uint32_t const latch = LoadU32(ctx, kResetFilterStateThrowLatch);
      StoreU32(ctx, kResetFilterStateThrowLatch, 0u);
      if (latch != 1u) {
        goto trap_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The node's dispatch slot, read through its vtable.
    target = LoadU32(ctx, LoadU32(ctx, entry) + 28u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstDispatchCall) : 1u) {
    CallTableT0(ctx, target, entry, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kFirstDispatchCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto walk_filters;
  }
second_dispatch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The alternate entry re-tests the base pointer the first dispatch left.
    target = static_cast<uint32_t>(entry == 0u);
  }
  if (target != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kResetFilterStateThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondAssertCall) : 1u) {
      HostFailAssertion(ctx, kResetFilterStateAssertLead, kResetFilterStateNullCheckAddr,
                        kResetFilterStateSharedPtrPathAddr, kResetFilterStateAssertLine,
                        kResetFilterStateAssertFuncAddr);
      resume_code = kSecondAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const latch = LoadU32(ctx, kResetFilterStateThrowLatch);
      StoreU32(ctx, kResetFilterStateThrowLatch, 0u);
      if (latch != 1u) {
        goto trap_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    target = LoadU32(ctx, LoadU32(ctx, entry) + 28u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondDispatchCall) : 1u) {
    CallTableT0(ctx, target, entry, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kSecondDispatchCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
walk_filters:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Set up the walk: the storage's slot base and its element count place the
    // first element, which is the bucket the count's remainder names.
    target = LoadU32(ctx, a + 2912u);
    count = LoadU32(ctx, a + 2924u);
    uint32_t const slot_index = count + LoadU32(ctx, a + 2928u);
    entry = DIV_U(slot_index, 93u);
    end_slot = target + (entry << 2u);
    element = 0u;
    self = a;
    begin_slot = LoadU32(ctx, a + 2916u);
    if (target != begin_slot) {
      element = LoadU32(ctx, end_slot) + (slot_index - entry * 93u) * 44u;
    }
    entry = element;
  }
next_filter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The slot pointer names the element: either the storage's base slot, or a
    // bucket lookup whose remainder picks the element inside that bucket.
    if (target == begin_slot) {
      element = 0u;
    } else {
      uint32_t const bucket = DIV_U(count, 93u);
      element = LoadU32(ctx, target + (bucket << 2u));
      element += (count - bucket * 93u) * 44u;
    }
    target = static_cast<uint32_t>(element == entry);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    walking = target;
  }
  if (walking != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The node storage every format below runs against.
      target = a + 2908u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInitNodeStorageCall) : 1u) {
      InitNodeStorage(ctx, target);
      resume_code = kInitNodeStorageCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The state word picks which base the tail formats against; 5 leaves the
      // walk at once.
      uint32_t const state_case = state_bits - 5u;
      if (state_case == 0u) {
        goto end_walk;
      }
      if (state_case == 1u) {
        goto second_base_path;
      }
      goto reset_done;
    }
  end_walk:;
  }
  if (walking == 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The line's two arguments: the object, and the slot one element back from
      // the walk -- clamped to the storage block when the walk ran off its end.
      element = a;
      target = entry;
      if (entry == LoadU32(ctx, end_slot)) {
        target = LoadU32(ctx, end_slot - 4u) + 4092u;
      }
      target -= 44u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstFormatLineCall) : 1u) {
      FormatLuaEventFilterLine(ctx, element, target);
      resume_code = kFirstFormatLineCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Step one element further through the storage and re-sample the block's
      // bounds; the walk repeats while elements are left.
      target = entry;
      if (entry == LoadU32(ctx, end_slot)) {
        end_slot -= 4u;
        target = LoadU32(ctx, end_slot) + 4092u;
      }
      entry = target - 44u;
      begin_slot = LoadU32(ctx, a + 2916u);
      count = LoadU32(ctx, a + 2924u);
      target = LoadU32(ctx, a + 2912u);
      goto next_filter;
    }
  }
  selected = a + 14072u;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto base_selected;
  }
second_base_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The other base layout the reset formats against.
    selected = a + 14160u;
  }
base_selected:;
  a = selected;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondFormatLineCall) : 1u) {
    uint32_t const line = FormatLuaEventFilterLine(ctx, self, a);
    resume_code = kSecondFormatLineCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    a = line;
  }
reset_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto done;
  }
trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The raised throw is looked up in the catch table and handed to the panic.
    a = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPanicCall) : 1u) {
    Panic(ctx, a);
    resume_code = kPanicCall;
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
    // the 40-byte frame a rewinding pass reads back.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, target);
    StoreU32(ctx, frame + 8u, entry);
    StoreU32(ctx, frame + 12u, element);
    StoreU32(ctx, frame + 16u, end_slot);
    StoreU32(ctx, frame + 20u, count);
    StoreU32(ctx, frame + 24u, begin_slot);
    StoreU32(ctx, frame + 28u, self);
    StoreU32(ctx, frame + 32u, walking);
    StoreU32(ctx, frame + 36u, state_bits);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
done:;
  LeaveFrame();
}

}  // namespace artc
