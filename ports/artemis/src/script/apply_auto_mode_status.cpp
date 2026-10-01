#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void UnorderedInsertBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);

namespace {

// The five words the failure report carries: the leading word, the "px != 0"
// expression, its boost source file, the line and the enclosing function.
constexpr uint32_t kApplyAutoModeStatusAssertLead = 857u;
constexpr uint32_t kApplyAutoModeStatusNullCheckAddr = 0xB5E31u;
constexpr uint32_t kApplyAutoModeStatusSharedPtrPathAddr = 0xB5E39u;
constexpr uint32_t kApplyAutoModeStatusAssertLine = 734u;
constexpr uint32_t kApplyAutoModeStatusAssertFuncAddr = 0xB5E71u;
// The name of the status the record below is built from, in the three pieces
// that are copied into it, and the empty tag every bucket insert is handed.
constexpr uint32_t kApplyAutoModeStatusNameAddr = 0xB4A1Cu;
constexpr uint32_t kApplyAutoModeStatusNameMidAddr = 0xB4A24u;
constexpr uint32_t kApplyAutoModeStatusNameTailAddr = 0xB4A2Cu;
constexpr uint32_t kApplyAutoModeStatusEmptyTagAddr = 0xB5E30u;
// The word and the empty string the invoke bridge is called with.
constexpr uint32_t kApplyAutoModeStatusInvokeArgAddr = 0xAF3EDu;
// The engine's pending-throw latch, cleared before every bridge call that can
// raise and read back once it returns; and the all-ones word the record's
// signed length field is compared against.
constexpr uint32_t kApplyAutoModeStatusThrowLatch = 0xD015Cu;
constexpr uint32_t kApplyAutoModeStatusSentinel = 0xFFFFFFFFu;
// The 64-bit initializer written into the record's second word.
constexpr uint64_t kApplyAutoModeStatusRecordInit = 9223372174293729297ull;

}  // namespace

// Applies the auto-mode status: the new flag is written into the object's status
// slot and the layer state follows it. The body is suspendable -- a rewinding
// pass restores the seven slots below from the frame it pushed and replays the
// one call its resume code names.
void ApplyAutoModeStatus(EngineContext* ctx, uint32_t object, uint32_t status, uint32_t holder) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 28u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the thirty-three call sites below spill.
  uint32_t const kFirstInsertCall = 0u;
  uint32_t const kSecondInsertCall = 1u;
  uint32_t const kFirstInvokeCall = 2u;
  uint32_t const kFirstAssertCall = 3u;
  uint32_t const kSecondAssertCall = 4u;
  uint32_t const kThirdInsertCall = 5u;
  uint32_t const kFirstStepCall = 6u;
  uint32_t const kFirstScheduleCall = 7u;
  uint32_t const kFirstPairRelease = 8u;
  uint32_t const kSecondPairRelease = 9u;
  uint32_t const kThirdPairRelease = 10u;
  uint32_t const kFourthPairRelease = 11u;
  uint32_t const kThirdAssertCall = 12u;
  uint32_t const kFirstAllocateCall = 13u;
  uint32_t const kFirstDispatchCall = 14u;
  uint32_t const kFirstFormatCall = 15u;
  uint32_t const kFourthInsertCall = 16u;
  uint32_t const kSecondInvokeCall = 17u;
  uint32_t const kFourthAssertCall = 18u;
  uint32_t const kFifthAssertCall = 19u;
  uint32_t const kFifthInsertCall = 20u;
  uint32_t const kSecondStepCall = 21u;
  uint32_t const kSecondScheduleCall = 22u;
  uint32_t const kFifthPairRelease = 23u;
  uint32_t const kSixthPairRelease = 24u;
  uint32_t const kSeventhPairRelease = 25u;
  uint32_t const kEighthPairRelease = 26u;
  uint32_t const kSixthAssertCall = 27u;
  uint32_t const kSecondAllocateCall = 28u;
  uint32_t const kSecondDispatchCall = 29u;
  uint32_t const kSecondFormatCall = 30u;
  uint32_t const kFirstHandoffCall = 31u;
  uint32_t const kSecondHandoffCall = 32u;
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it: object, status
  // and holder are the call's own arguments, and resume_code, carry, stack_slot,
  // scratch and slot_ptr are the walk's. The status slot starts as the flag the
  // call applies and is reused as the owner pair it unwinds afterwards.
  uint32_t resume_code = 0u;
  uint32_t carry = 0u;
  uint32_t stack_slot = 0u;
  uint32_t scratch = 0u;
  uint32_t slot_ptr = 0u;
  uint32_t call_result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path pushed and rebuild the
    // seven saved slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    object = LoadU32(ctx, frame);
    status = LoadU32(ctx, frame + 4u);
    holder = LoadU32(ctx, frame + 8u);
    carry = LoadU32(ctx, frame + 12u);
    stack_slot = LoadU32(ctx, frame + 16u);
    scratch = LoadU32(ctx, frame + 20u);
    slot_ptr = LoadU32(ctx, frame + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay
    // separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 32-byte guest-stack record carries the status name and its two argument
    // words to the calls below.
    stack_slot = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = stack_slot;
    carry = static_cast<uint32_t>(status == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Nothing to apply when the flag is clear, there is no layer pointer and the
    // object's flag byte is already clear; and nothing to do when the recorded
    // flag is the one being passed.
    if (status != 0u && holder == 0u && LoadU8(ctx, object + 34u) == 0u) {
      goto finish;
    }
    if (LoadU8(ctx, object + 35u) == status) {
      goto finish;
    }
    StoreU8(ctx, object + 35u, status);
    holder = object + 13308u;
  }
  if (status || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The holder is parked in the record's last word and the object's own word
      // at +36 becomes the first argument of the insert below.
      StoreU32(ctx, stack_slot + 24u, holder);
      carry = stack_slot + 24u;
      scratch = stack_slot + 16u;
      status = object + 36u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstInsertCall) : 1u) {
      UnorderedInsertBucketNode(ctx, stack_slot, status, holder, kApplyAutoModeStatusEmptyTagAddr, carry, scratch);
      resume_code = kFirstInsertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The entry that came back is asked whether it is still unset.
      carry = static_cast<uint32_t>(LoadU8(ctx, LoadU32(ctx, stack_slot) + 32u) == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Already set: the pair release chain below is skipped.
      if (carry) {
        goto pair_release_done;
      }
      StoreU32(ctx, stack_slot + 24u, holder);
      scratch = stack_slot + 16u;
      carry = stack_slot + 24u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondInsertCall) : 1u) {
      UnorderedInsertBucketNode(ctx, stack_slot, status, holder, kApplyAutoModeStatusEmptyTagAddr, carry, scratch);
      resume_code = kSecondInsertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The record's byte at +47 is a signed count: a negative one selects the
      // field at +40, and the result is tested for zero.
      carry = LoadU32(ctx, stack_slot);
      slot_ptr = LoadU32(ctx, carry + 40u);
      scratch = LoadU8(ctx, carry + 47u);
      call_result = static_cast<uint32_t>(static_cast<int8_t>(scratch) < 0);
      scratch = static_cast<uint32_t>((call_result ? slot_ptr : scratch) == 0u);
    }
    if (scratch || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
        carry = carry + 36u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstInvokeCall) : 1u) {
        call_result = HostInvokeIiiiii(ctx, 1354u, carry, 0u, kApplyAutoModeStatusSentinel,
                                       kApplyAutoModeStatusInvokeArgAddr, 0u);
        carry = call_result;
        resume_code = kFirstInvokeCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // The invoke raised: hand the in-flight exception over. A zero result
        // means the entry is gone, so the pair release chain is skipped.
        scratch = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
        scratch = static_cast<uint32_t>(scratch == 1u);
        if (scratch) {
          goto catch_path;
        }
        carry = static_cast<uint32_t>(carry == 0u);
        if (carry) {
          goto pair_release_done;
        }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, object + 16u);
      scratch = static_cast<uint32_t>(carry == 0u);
    }
    if (scratch || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstAssertCall) : 1u) {
        HostFailAssertion(ctx, kApplyAutoModeStatusAssertLead, kApplyAutoModeStatusNullCheckAddr,
                          kApplyAutoModeStatusSharedPtrPathAddr, kApplyAutoModeStatusAssertLine,
                          kApplyAutoModeStatusAssertFuncAddr);
        resume_code = kFirstAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        object = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
        object = static_cast<uint32_t>(object != 1u);
        if (object) {
          goto no_throw_path;
        }
        goto handoff_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU32(ctx, carry + 24u);
      scratch = static_cast<uint32_t>(carry == 0u);
    }
    if (scratch || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondAssertCall) : 1u) {
        HostFailAssertion(ctx, kApplyAutoModeStatusAssertLead, kApplyAutoModeStatusNullCheckAddr,
                          kApplyAutoModeStatusSharedPtrPathAddr, kApplyAutoModeStatusAssertLine,
                          kApplyAutoModeStatusAssertFuncAddr);
        resume_code = kSecondAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        object = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
        object = static_cast<uint32_t>(object != 1u);
        if (object) {
          goto no_throw_path;
        }
        goto handoff_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, stack_slot + 24u, holder);
      slot_ptr = stack_slot + 16u;
      scratch = stack_slot + 24u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdInsertCall) : 1u) {
      UnorderedInsertBucketNode(ctx, stack_slot, status, holder, kApplyAutoModeStatusEmptyTagAddr, scratch, slot_ptr);
      resume_code = kThirdInsertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Run the layer's row call: the record is the first argument, the object's
      // own word at +36 the second.
      holder = LoadU32(ctx, LoadU32(ctx, carry) + 304u);
      status = LoadU32(ctx, stack_slot) + 36u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstStepCall) : 1u) {
      CallTableT5(ctx, holder, stack_slot, carry, status);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kFirstStepCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The record's first word is the object the row call answered with; when
      // there is one, the pair it points at is unwound, otherwise the walk moves
      // on with a null holder.
      status = LoadU32(ctx, stack_slot + 4u);
      carry = 0u;
      holder = LoadU32(ctx, stack_slot);
      scratch = static_cast<uint32_t>(holder == 0u);
      if (scratch) {
        holder = carry;
      } else {
        holder = LoadU32(ctx, LoadU32(ctx, holder) - 152u) + holder;
      }
      carry = static_cast<uint32_t>(holder == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A null holder has nothing to schedule.
      if (carry) {
        goto owner_pair_path;
      }
      carry = LoadU32(ctx, LoadU32(ctx, holder) + 124u);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstScheduleCall) : 1u) {
      HostOsSchedule(ctx, carry, holder, 1u);
      resume_code = kFirstScheduleCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A raised schedule goes to the owner pair path below; otherwise the
      // object's own pair is stepped down here.
      holder = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      holder = static_cast<uint32_t>(holder != 1u);
      if (holder) {
        goto owner_pair_path;
      }
      object = HostRet0(ctx);
      (void)HostTempRet0(ctx);
      if (status == 0u) {
        goto os_release_path;
      }
      holder = LoadU32(ctx, status + 4u);
      carry = holder - 1u;
      StoreU32(ctx, status + 4u, carry);
      if (holder != 1u) {
        goto os_release_path;
      }
      holder = LoadU32(ctx, LoadU32(ctx, status) + 8u);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, holder, status);
      resume_code = kFirstPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      holder = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      holder = static_cast<uint32_t>(holder != 1u);
    }
    if (holder || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        holder = LoadU32(ctx, status + 8u);
        carry = holder - 1u;
        StoreU32(ctx, status + 8u, carry);
        if (holder != 1u) {
          goto os_release_path;
        }
        holder = LoadU32(ctx, LoadU32(ctx, status) + 12u);
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondPairRelease) : 1u) {
        HostReleaseSlotPair(ctx, holder, status);
        resume_code = kSecondPairRelease;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        status = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
        status = static_cast<uint32_t>(status != 1u);
        if (status) {
          goto os_release_path;
        }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto handoff_path;
    }
  owner_pair_path:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The owner pair is unwound here instead, the same way.
      holder = static_cast<uint32_t>(status == 0u);
      if (holder) {
        goto pair_release_done;
      }
      holder = LoadU32(ctx, status + 4u);
      carry = holder - 1u;
      StoreU32(ctx, status + 4u, carry);
      holder = static_cast<uint32_t>(holder != 1u);
      if (holder) {
        goto pair_release_done;
      }
      holder = LoadU32(ctx, LoadU32(ctx, status) + 8u);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, holder, status);
      resume_code = kThirdPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      holder = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      if (holder == 1u) {
        goto handoff_path;
      }
      holder = LoadU32(ctx, status + 8u);
      carry = holder - 1u;
      StoreU32(ctx, status + 8u, carry);
      holder = static_cast<uint32_t>(holder != 1u);
      if (holder) {
        goto pair_release_done;
      }
      holder = LoadU32(ctx, LoadU32(ctx, status) + 12u);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFourthPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, holder, status);
      resume_code = kFourthPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      status = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      status = static_cast<uint32_t>(status == 1u);
      if (status) {
        goto handoff_path;
      }
    }
  pair_release_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The layer pointer on the object has to be there, or the null check
      // below has to trip.
      holder = LoadU32(ctx, object + 112u);
      status = static_cast<uint32_t>(holder == 0u);
    }
    if (status || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdAssertCall) : 1u) {
        HostFailAssertion(ctx, kApplyAutoModeStatusAssertLead, kApplyAutoModeStatusNullCheckAddr,
                          kApplyAutoModeStatusSharedPtrPathAddr, kApplyAutoModeStatusAssertLine,
                          kApplyAutoModeStatusAssertFuncAddr);
        resume_code = kThirdAssertCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        object = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
        StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
        object = static_cast<uint32_t>(object != 1u);
        if (object) {
          goto no_throw_path;
        }
        goto handoff_path;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstAllocateCall) : 1u) {
      call_result = AllocateStringBuffer(ctx, 32u);
      status = call_result;
      resume_code = kFirstAllocateCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The fresh buffer becomes a record holding the status name and the parked
      // arguments, and the layer's word at +20 becomes the call index.
      StoreU32(ctx, stack_slot, status);
      StoreU64(ctx, stack_slot + 4u, kApplyAutoModeStatusRecordInit);
      StoreU8(ctx, status + 17u, 0u);
      carry = LoadU8(ctx, kApplyAutoModeStatusNameTailAddr);
      StoreU8(ctx, status + 16u, carry);
      StoreU64(ctx, status + 8u, LoadU64(ctx, kApplyAutoModeStatusNameMidAddr));
      StoreU64(ctx, status, LoadU64(ctx, kApplyAutoModeStatusNameAddr));
      status = LoadU32(ctx, LoadU32(ctx, holder) + 20u);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstDispatchCall) : 1u) {
      call_result = HostDispatchInvoke(ctx, status, holder, stack_slot, 1u);
      status = call_result;
      resume_code = kFirstDispatchCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      status = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      status = static_cast<uint32_t>(status != 1u);
    }
    if (status || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // A record whose signed length field is not negative had a buffer to
        // release, and the object's own word at +13764 becomes the argument of
        // the line call below.
        if (static_cast<int32_t>(LoadS8(ctx, stack_slot + 11u)) <= static_cast<int32_t>(kApplyAutoModeStatusSentinel)) {
          DeallocateRawBlock(ctx, LoadU32(ctx, stack_slot));
        }
        status = object + 13764u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstFormatCall) : 1u) {
        call_result = FormatLuaEventFilterLine(ctx, object, status);
        object = call_result;
        resume_code = kFirstFormatCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        goto finish;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The line call's answer is in the runner's return slot; the record's
      // buffer is released and the object handed over to the OS release bridge.
      object = HostRet0(ctx);
      (void)HostTempRet0(ctx);
      if (static_cast<int32_t>(LoadS8(ctx, stack_slot + 11u)) > static_cast<int32_t>(kApplyAutoModeStatusSentinel)) {
        goto os_release_path;
      }
      DeallocateRawBlock(ctx, LoadU32(ctx, stack_slot));
      HostOsRelease(ctx, object);
      TrapUnreachable();
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The status flag is clear: the same record is built once more, against the
    // object's own word at +36.
    StoreU32(ctx, stack_slot + 24u, holder);
    carry = object + 36u;
    scratch = stack_slot + 16u;
    status = stack_slot + 24u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFourthInsertCall) : 1u) {
    UnorderedInsertBucketNode(ctx, stack_slot, carry, holder, kApplyAutoModeStatusEmptyTagAddr, status, scratch);
    resume_code = kFourthInsertCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, stack_slot);
    slot_ptr = LoadU32(ctx, status + 40u);
    scratch = LoadU8(ctx, status + 47u);
    call_result = static_cast<uint32_t>(static_cast<int8_t>(scratch) < 0);
    scratch = static_cast<uint32_t>((call_result ? slot_ptr : scratch) == 0u);
  }
  if (scratch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      status = status + 36u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondInvokeCall) : 1u) {
      call_result = HostInvokeIiiiii(ctx, 1354u, status, 0u, kApplyAutoModeStatusSentinel,
                                     kApplyAutoModeStatusInvokeArgAddr, 0u);
      status = call_result;
      resume_code = kSecondInvokeCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      scratch = static_cast<uint32_t>(scratch == 1u);
      if (scratch) {
        goto handoff_path;
      }
      status = static_cast<uint32_t>(status == 0u);
      if (status) {
        goto second_pair_release_done;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, object + 16u);
    scratch = static_cast<uint32_t>(status == 0u);
  }
  if (scratch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFourthAssertCall) : 1u) {
      HostFailAssertion(ctx, kApplyAutoModeStatusAssertLead, kApplyAutoModeStatusNullCheckAddr,
                        kApplyAutoModeStatusSharedPtrPathAddr, kApplyAutoModeStatusAssertLine,
                        kApplyAutoModeStatusAssertFuncAddr);
      resume_code = kFourthAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      object = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      object = static_cast<uint32_t>(object != 1u);
      if (object) {
        goto no_throw_path;
      }
      goto handoff_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, status + 24u);
    scratch = static_cast<uint32_t>(status == 0u);
  }
  if (scratch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFifthAssertCall) : 1u) {
      HostFailAssertion(ctx, kApplyAutoModeStatusAssertLead, kApplyAutoModeStatusNullCheckAddr,
                        kApplyAutoModeStatusSharedPtrPathAddr, kApplyAutoModeStatusAssertLine,
                        kApplyAutoModeStatusAssertFuncAddr);
      resume_code = kFifthAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      object = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      object = static_cast<uint32_t>(object != 1u);
      if (object) {
        goto no_throw_path;
      }
      goto handoff_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, stack_slot + 24u, holder);
    slot_ptr = stack_slot + 16u;
    scratch = stack_slot + 24u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFifthInsertCall) : 1u) {
    UnorderedInsertBucketNode(ctx, stack_slot, carry, holder, kApplyAutoModeStatusEmptyTagAddr, scratch, slot_ptr);
    resume_code = kFifthInsertCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, LoadU32(ctx, status) + 304u);
    holder = LoadU32(ctx, stack_slot) + 36u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondStepCall) : 1u) {
    CallTableT5(ctx, carry, stack_slot, status, holder);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kSecondStepCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, stack_slot + 4u);
    carry = 0u;
    holder = LoadU32(ctx, stack_slot);
    if (holder) {
      holder = LoadU32(ctx, LoadU32(ctx, holder) - 152u) + holder;
    } else {
      holder = carry;
    }
    carry = static_cast<uint32_t>(holder == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (carry) {
      goto second_owner_pair_path;
    }
    carry = LoadU32(ctx, LoadU32(ctx, holder) + 124u);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondScheduleCall) : 1u) {
    HostOsSchedule(ctx, carry, holder, 0u);
    resume_code = kSecondScheduleCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    holder = static_cast<uint32_t>(holder != 1u);
    if (holder) {
      goto second_owner_pair_path;
    }
    object = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    if (status == 0u) {
      goto os_release_path;
    }
    holder = LoadU32(ctx, status + 4u);
    StoreU32(ctx, status + 4u, holder - 1u);
    if (holder != 1u) {
      goto os_release_path;
    }
    holder = LoadU32(ctx, LoadU32(ctx, status) + 8u);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFifthPairRelease) : 1u) {
    HostReleaseSlotPair(ctx, holder, status);
    resume_code = kFifthPairRelease;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    holder = static_cast<uint32_t>(holder != 1u);
  }
  if (holder || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      holder = LoadU32(ctx, status + 8u);
      StoreU32(ctx, status + 8u, holder - 1u);
      if (holder != 1u) {
        goto os_release_path;
      }
      holder = LoadU32(ctx, LoadU32(ctx, status) + 12u);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSixthPairRelease) : 1u) {
      HostReleaseSlotPair(ctx, holder, status);
      resume_code = kSixthPairRelease;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      status = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      status = static_cast<uint32_t>(status != 1u);
      if (status) {
        goto os_release_path;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto handoff_path;
  }
second_owner_pair_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = static_cast<uint32_t>(status == 0u);
    if (holder) {
      goto second_pair_release_done;
    }
    holder = LoadU32(ctx, status + 4u);
    StoreU32(ctx, status + 4u, holder - 1u);
    holder = static_cast<uint32_t>(holder != 1u);
    if (holder) {
      goto second_pair_release_done;
    }
    holder = LoadU32(ctx, LoadU32(ctx, status) + 8u);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSeventhPairRelease) : 1u) {
    HostReleaseSlotPair(ctx, holder, status);
    resume_code = kSeventhPairRelease;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    if (holder == 1u) {
      goto handoff_path;
    }
    holder = LoadU32(ctx, status + 8u);
    StoreU32(ctx, status + 8u, holder - 1u);
    holder = static_cast<uint32_t>(holder != 1u);
    if (holder) {
      goto second_pair_release_done;
    }
    holder = LoadU32(ctx, LoadU32(ctx, status) + 12u);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEighthPairRelease) : 1u) {
    HostReleaseSlotPair(ctx, holder, status);
    resume_code = kEighthPairRelease;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    status = static_cast<uint32_t>(status == 1u);
    if (status) {
      goto handoff_path;
    }
  }
second_pair_release_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = LoadU32(ctx, object + 112u);
    status = static_cast<uint32_t>(holder == 0u);
  }
  if (status || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSixthAssertCall) : 1u) {
      HostFailAssertion(ctx, kApplyAutoModeStatusAssertLead, kApplyAutoModeStatusNullCheckAddr,
                        kApplyAutoModeStatusSharedPtrPathAddr, kApplyAutoModeStatusAssertLine,
                        kApplyAutoModeStatusAssertFuncAddr);
      resume_code = kSixthAssertCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      object = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
      StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
      object = static_cast<uint32_t>(object != 1u);
      if (object) {
        goto no_throw_path;
      }
      goto handoff_path;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondAllocateCall) : 1u) {
    call_result = AllocateStringBuffer(ctx, 32u);
    status = call_result;
    resume_code = kSecondAllocateCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, stack_slot, status);
    StoreU64(ctx, stack_slot + 4u, kApplyAutoModeStatusRecordInit);
    StoreU8(ctx, status + 17u, 0u);
    StoreU8(ctx, status + 16u, LoadU8(ctx, kApplyAutoModeStatusNameTailAddr));
    StoreU64(ctx, status + 8u, LoadU64(ctx, kApplyAutoModeStatusNameMidAddr));
    StoreU64(ctx, status, LoadU64(ctx, kApplyAutoModeStatusNameAddr));
    status = LoadU32(ctx, LoadU32(ctx, holder) + 20u);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondDispatchCall) : 1u) {
    call_result = HostDispatchInvoke(ctx, status, holder, stack_slot, 0u);
    status = call_result;
    resume_code = kSecondDispatchCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, kApplyAutoModeStatusThrowLatch);
    StoreU32(ctx, kApplyAutoModeStatusThrowLatch, 0u);
    if (status == 1u) {
      goto answer_path;
    }
    // A record whose signed length field is not negative had a buffer to
    // release. Then the object's five flag bytes decide whether the status word
    // at +2768 is cleared, and its word at +13808 becomes the next argument.
    if (static_cast<int32_t>(LoadS8(ctx, stack_slot + 11u)) <= static_cast<int32_t>(kApplyAutoModeStatusSentinel)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, stack_slot));
    }
    if (LoadU8(ctx, object + 2772u) != 0u) {
      goto status_clear_done;
    }
    if (LoadU8(ctx, object + 2773u) != 0u) {
      goto status_clear_done;
    }
    if (LoadU8(ctx, object + 2774u) != 0u) {
      goto status_clear_done;
    }
    if (LoadU8(ctx, object + 2775u) != 0u) {
      goto status_clear_done;
    }
    if (LoadU8(ctx, object + 2792u) != 0u) {
      goto status_clear_done;
    }
    StoreU32(ctx, object + 2768u, 0u);
  status_clear_done:;
    status = object + 13808u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondFormatCall) : 1u) {
    call_result = FormatLuaEventFilterLine(ctx, object, status);
    object = call_result;
    resume_code = kSecondFormatCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
finish:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = stack_slot + 32u;
    goto exit_path;
  }
answer_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The dispatch raised: the answer is taken from the runner's return slot and
    // the record's buffer released.
    object = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, stack_slot + 11u)) > static_cast<int32_t>(kApplyAutoModeStatusSentinel)) {
      goto os_release_path;
    }
    DeallocateRawBlock(ctx, LoadU32(ctx, stack_slot));
  }
os_release_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, object);
    TrapUnreachable();
  }
catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Ask the engine which catch the in-flight exception matches -- there is no
    // type filter at this site -- and let the bridge read the runner's return
    // slot.
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
  goto exit_path;
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
    StoreU32(ctx, frame + 4u, status);
    StoreU32(ctx, frame + 8u, holder);
    StoreU32(ctx, frame + 12u, carry);
    StoreU32(ctx, frame + 16u, stack_slot);
    StoreU32(ctx, frame + 20u, scratch);
    StoreU32(ctx, frame + 24u, slot_ptr);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
