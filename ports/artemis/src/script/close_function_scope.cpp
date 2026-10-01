#include "util/engine_context.h"

namespace artc {

// Forward declarations -- registered callees.
void ResizeTableArray(EngineContext* ctx, uint32_t a, uint32_t b);
void ClearLocalFlags(EngineContext* ctx, uint32_t a);
void ClearUpvalueFlags(EngineContext* ctx, uint32_t a);
void InvokeMetaField(EngineContext* ctx, uint32_t a);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t LoadProtoSlot(EngineContext* ctx, uint32_t a);
uint32_t ReshapeTaggedNodeChain(EngineContext* ctx, uint32_t state, uint32_t node, uint32_t tag);

namespace {

// The tag the closing chain is reshaped with, and the all-ones word the block
// slot mask is built from.
constexpr uint32_t kCloseFunctionScopeTag = 0xFFFFFFFDu;
constexpr uint32_t kCloseFunctionScopeAllOnes = 0xFFFFFFFFu;

}  // namespace

// Closes the function scope: runs the pending close metamethods, unwinds the
// open-upvalue and local chains, drops the scopes it held and releases the
// blocks it no longer needs. Returns the stage code the teardown is resumed at.
// The body is suspendable -- a rewinding pass restores the slots below from the
// frame it pushed and replays the one call its resume code names.
uint32_t CloseFunctionScope(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 32u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kSlotWordBytes = 4u;
  uint32_t const kScopeProtoCall = 0u;
  uint32_t const kFirstChainProtoCall = 1u;
  uint32_t const kSecondChainProtoCall = 2u;
  uint32_t const kThirdChainProtoCall = 3u;
  uint32_t const kTotalProtoCall = 4u;
  uint32_t const kFirstReshapeCall = 5u;
  uint32_t const kSecondReshapeCall = 6u;
  uint32_t const kResizeTableCall = 7u;
  uint32_t const kReallocateCall = 8u;
  uint32_t const kInvokeMetaCall = 9u;
  uint32_t const kStageTwoCode = 10u;
  uint32_t const kStageThreeCode = 400u;
  uint32_t const kStageFourCode = 100u;
  uint32_t cursor = 0u;
  uint32_t func_state = 0u;
  uint32_t scratch = 0u;
  uint32_t work = 0u;
  uint32_t value = 0u;
  uint32_t total = 0u;
  uint32_t extra = 0u;
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  // Rewinding: pop the 32-byte frame the suspend path pushed and rebuild the
  // eight slots it saved.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    cursor = LoadU32(ctx, frame + 4u);
    func_state = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
    work = LoadU32(ctx, frame + 16u);
    value = LoadU32(ctx, frame + 20u);
    total = LoadU32(ctx, frame + 24u);
    extra = LoadU32(ctx, frame + 28u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight when the frame was pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    func_state = LoadU32(ctx, state + 16u);
    scratch = LoadU8(ctx, func_state + 21u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (scratch) {
      case 0: goto clear_upvalues;
      case 1: goto open_upvalues;
      case 2: goto stage_two;
      case 3: goto stage_three;
      case 4: goto stage_four;
      default: goto return_step;
    }
  clear_upvalues:;
    ClearUpvalueFlags(ctx, state);
    result = 0u;
    goto exit_path;
  open_upvalues:;
    cursor = LoadU32(ctx, func_state + 36u);
  }
  if (cursor || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kScopeProtoCall) : 1u) {
      uint32_t const proto = LoadProtoSlot(ctx, func_state);
      resume_code = kScopeProtoCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      state = proto;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      result = state;
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, func_state + 140u);
    scratch = func_state + 120u;
    work = static_cast<uint32_t>(cursor == scratch);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (work != 0u) {
      goto walk_done;
    }
    do {
      if ((LoadU8(ctx, cursor + 5u) & 7u) == 0u) {
        value = LoadU32(ctx, cursor + 8u);
        if (static_cast<int32_t>(LoadU32(ctx, value + 8u)) >= 4) {
          value = LoadU32(ctx, value);
          if ((LoadU8(ctx, value + 5u) & 3u) != 0u) {
            ClearFlagsRecursive(ctx, func_state, value);
          }
        }
      }
      uint32_t const next = LoadU32(ctx, cursor + 20u);
      cursor = next;
      work = static_cast<uint32_t>(scratch != next);
    } while (work != 0u);
    cursor = static_cast<uint32_t>(LoadU32(ctx, func_state + 36u) == 0u);
    if (cursor != 0u) {
      goto walk_done;
    }
  }
  while (true) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstChainProtoCall) : 1u) {
      uint32_t const proto = LoadProtoSlot(ctx, func_state);
      resume_code = kFirstChainProtoCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      cursor = proto;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = LoadU32(ctx, func_state + 36u);
      if (cursor != 0u) {
        continue;
      }
    }
    break;
  }
  // Merge point: every open-upvalue chain has been released.
  walk_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, func_state + 44u);
    StoreU32(ctx, func_state + 44u, 0u);
    StoreU32(ctx, func_state + 36u, cursor);
    if ((LoadU8(ctx, state + 5u) & 3u) != 0u) {
      ClearFlagsRecursive(ctx, func_state, state);
    }
    ClearLocalFlags(ctx, func_state);
    cursor = LoadU32(ctx, func_state + 36u);
  }
  if (cursor || ResumePhase(ctx) == kResumeRewinding) {
    while (true) {
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondChainProtoCall) : 1u) {
        uint32_t const proto = LoadProtoSlot(ctx, func_state);
        resume_code = kSecondChainProtoCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        cursor = proto;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        cursor = LoadU32(ctx, func_state + 36u);
        if (cursor != 0u) {
          continue;
        }
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, func_state + 40u);
    StoreU32(ctx, func_state + 40u, 0u);
    StoreU32(ctx, func_state + 36u, cursor);
  }
  if (cursor || ResumePhase(ctx) == kResumeRewinding) {
    while (true) {
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdChainProtoCall) : 1u) {
        uint32_t const proto = LoadProtoSlot(ctx, func_state);
        resume_code = kThirdChainProtoCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        cursor = proto;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        cursor = LoadU32(ctx, func_state + 36u);
        if (cursor != 0u) {
          continue;
        }
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    total = LoadU32(ctx, state + 16u);
    scratch = LoadU32(ctx, total + 112u);
    cursor = LoadU32(ctx, scratch);
    if (cursor != 0u) {
      do {
        // A node needs closing only when it carries a kind flag and is not
        // already marked as closing.
        value = LoadU8(ctx, cursor + 5u);
        work = static_cast<uint32_t>((value & 3u) == 0u || (value & 8u) != 0u);
        if (work != 0u) {
          scratch = cursor;
          goto node_done;
        }
        work = LoadU32(ctx, cursor + 8u);
        if (work == 0u) {
          goto no_close_meta;
        }
        if ((LoadU8(ctx, work + 6u) & 4u) != 0u) {
          goto no_close_meta;
        }
        work = FetchMetamethod(ctx, work, 2u, LoadU32(ctx, LoadU32(ctx, state + 16u) + 196u));
        if (work != 0u) {
          goto close_node;
        }
      no_close_meta:;
        value = LoadU8(ctx, cursor + 5u);
        StoreU8(ctx, cursor + 5u, value | 8u);
        scratch = cursor;
        goto node_done;
      close_node:;
        // Close it now: mark it, unlink it from the chain and account for its size.
        StoreU8(ctx, cursor + 5u, LoadU8(ctx, cursor + 5u) | 8u);
        value = LoadU32(ctx, cursor + 16u);
        StoreU32(ctx, scratch, LoadU32(ctx, cursor));
        extra = extra + value + 24u;
        value = LoadU32(ctx, total + 48u);
        work = static_cast<uint32_t>(value == 0u);
        if (work != 0u) {
          StoreU32(ctx, cursor, cursor);
        } else {
          StoreU32(ctx, cursor, LoadU32(ctx, value));
          work = LoadU32(ctx, total + 48u);
          StoreU32(ctx, work, cursor);
        }
        StoreU32(ctx, total + 48u, cursor);
      node_done:;
        cursor = LoadU32(ctx, scratch);
      } while (cursor != 0u);
    }
    cursor = LoadU32(ctx, func_state + 48u);
    while (cursor != 0u) {
      cursor = LoadU32(ctx, cursor);
      scratch = LoadU8(ctx, cursor + 5u) & 248u;
      state = scratch | (LoadU8(ctx, func_state + 20u) & 3u);
      StoreU8(ctx, cursor + 5u, state);
      ClearFlagsRecursive(ctx, func_state, cursor);
      if (cursor == LoadU32(ctx, func_state + 48u)) {
        break;
      }
    }
    total = 0u;
    state = LoadU32(ctx, func_state + 36u);
  }
  if (state || ResumePhase(ctx) == kResumeRewinding) {
    while (true) {
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTotalProtoCall) : 1u) {
        uint32_t const proto = LoadProtoSlot(ctx, func_state);
        resume_code = kTotalProtoCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        state = proto;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        total = state + total;
        state = LoadU32(ctx, func_state + 36u);
        if (state != 0u) {
          continue;
        }
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = LoadU32(ctx, func_state + 44u);
    if (scratch != 0u) {
      do {
        cursor = LoadU32(ctx, scratch + 28u);
        if (cursor == 0u) {
          goto close_slots;
        }
        if ((LoadU8(ctx, scratch + 5u) & 16u) == 0u) {
          goto close_slots;
        }
        // Close the slots the scope still holds, newest first.
        do {
          cursor = cursor - 1u;
          state = LoadU32(ctx, scratch + 12u) + (cursor << 4u);
          value = LoadU32(ctx, state + 8u);
          if (static_cast<int32_t>(value) >= 4) {
            if (value == 4u) {
              state = LoadU32(ctx, state);
              work = LoadU8(ctx, state + 5u) & 252u;
              StoreU8(ctx, state + 5u, work);
            } else {
              work = LoadU8(ctx, LoadU32(ctx, state) + 5u);
              if ((work & 3u) != 0u || (value == 7u && (work & 8u) != 0u)) {
                StoreU32(ctx, state + 8u, 0u);
              }
            }
          }
        } while (cursor != 0u);
      close_slots:;
        state = (kCloseFunctionScopeAllOnes << (LoadU8(ctx, scratch + 7u) & 31u)) ^
                kCloseFunctionScopeAllOnes;
        do {
          cursor = state;
          state = LoadU32(ctx, scratch + 16u) + (cursor << 5u);
          value = LoadU32(ctx, state + 8u);
          if (value == 0u) {
            goto next_block_slot;
          }
          work = LoadU32(ctx, state + 24u);
          if (static_cast<int32_t>(work) < 4) {
            goto check_block_value;
          }
          if (work == 4u) {
            value = LoadU32(ctx, state + 16u);
            work = LoadU8(ctx, value + 5u) & 252u;
            StoreU8(ctx, value + 5u, work);
            value = LoadU32(ctx, state + 8u);
            goto check_block_value;
          }
          if ((LoadU8(ctx, LoadU32(ctx, state + 16u) + 5u) & 3u) != 0u) {
            goto clear_block_slot;
          }
        check_block_value:;
          if (static_cast<int32_t>(value) < 4) {
            goto next_block_slot;
          }
          if (value == 4u) {
            state = LoadU32(ctx, state);
            work = LoadU8(ctx, state + 5u) & 252u;
            StoreU8(ctx, state + 5u, work);
            goto next_block_slot;
          }
          work = LoadU8(ctx, LoadU32(ctx, state) + 5u);
          if ((work & 3u) == 0u && (value != 7u || (work & 8u) == 0u)) {
            goto next_block_slot;
          }
        clear_block_slot:;
          StoreU32(ctx, state + 8u, 0u);
          if (static_cast<int32_t>(LoadU32(ctx, state + 24u)) >= 4) {
            StoreU32(ctx, state + 24u, 11u);
          }
        next_block_slot:;
          state = cursor - 1u;
        } while (cursor != 0u);
        scratch = LoadU32(ctx, scratch + 24u);
      } while (scratch != 0u);
    }
    StoreU32(ctx, func_state + 24u, 0u);
    StoreU8(ctx, func_state + 21u, 2u);
    StoreU32(ctx, func_state + 32u, func_state + 28u);
    StoreU8(ctx, func_state + 20u, LoadU8(ctx, func_state + 20u) ^ 3u);
    StoreU32(ctx, func_state + 72u,
             LoadU32(ctx, func_state + 68u) - (total + extra));
    result = 0u;
    goto exit_path;
  }
  stage_two:;
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, func_state + 24u);
    StoreU32(ctx, func_state + 24u, cursor + 1u);
    scratch = LoadU32(ctx, func_state + 68u);
    value = LoadU32(ctx, func_state);
    cursor = value + (cursor << 2u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstReshapeCall) : 1u) {
    uint32_t const reshaped = ReshapeTaggedNodeChain(ctx, state, cursor, kCloseFunctionScopeTag);
    resume_code = kFirstReshapeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    state = reshaped;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadU32(ctx, func_state + 24u)) >=
        static_cast<int32_t>(LoadU32(ctx, func_state + 8u))) {
      StoreU8(ctx, func_state + 21u, 3u);
    }
    StoreU32(ctx, func_state + 72u,
             LoadU32(ctx, func_state + 72u) + (LoadU32(ctx, func_state + 68u) - scratch));
    result = kStageTwoCode;
    goto exit_path;
  }
  stage_three:;
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, func_state + 68u);
    cursor = LoadU32(ctx, func_state + 32u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondReshapeCall) : 1u) {
    uint32_t const reshaped = ReshapeTaggedNodeChain(ctx, state, cursor, 40u);
    resume_code = kSecondReshapeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    cursor = reshaped;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 32u, cursor);
    cursor = static_cast<uint32_t>(LoadU32(ctx, cursor) == 0u);
  }
  if (cursor || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = LoadU32(ctx, state + 16u);
      scratch = LoadU32(ctx, cursor + 8u);
      extra = I32_DIV_S(scratch, kSlotWordBytes);
      work = static_cast<uint32_t>(static_cast<int32_t>(scratch) < 65);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (work != 0u) {
        goto size_ready;
      }
      work = static_cast<uint32_t>(extra <= LoadU32(ctx, cursor + 4u));
      if (work != 0u) {
        goto size_ready;
      }
      scratch = scratch >> 1u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kResizeTableCall) : 1u) {
      ResizeTableArray(ctx, state, scratch);
      resume_code = kResizeTableCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    // Merge point: the slot table is either resized or already large enough.
    size_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, cursor + 60u);
      work = static_cast<uint32_t>(scratch >= 65u);
    }
    if (work || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        work = state;
        total = LoadU32(ctx, cursor + 52u);
        state = scratch >> 1u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReallocateCall) : 1u) {
        uint32_t const block = ReallocateBlock(ctx, work, total, scratch, state);
        resume_code = kReallocateCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        scratch = block;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, cursor + 60u, state);
        StoreU32(ctx, cursor + 52u, scratch);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, func_state + 21u, 4u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 72u,
             LoadU32(ctx, func_state + 72u) + (LoadU32(ctx, func_state + 68u) - value));
    result = kStageThreeCode;
    goto exit_path;
  }
  stage_four:;
  // A resumed pass keeps the list head it saved; the running pass reads it back.
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = LoadU32(ctx, func_state + 48u);
  }
  if (scratch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInvokeMetaCall) : 1u) {
      InvokeMetaField(ctx, state);
      resume_code = kInvokeMetaCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = kStageFourCode;
      state = LoadU32(ctx, func_state + 72u);
      if (state < kStageFourCode + 1u) {
        goto return_step;
      }
      StoreU32(ctx, func_state + 72u, state - kStageFourCode);
      result = kStageFourCode;
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 76u, 0u);
    StoreU8(ctx, func_state + 21u, 0u);
  }
  // Merge point: the teardown reports the cursor it stopped at.
  return_step:;
  if (ResumePhase(ctx) == kResumeRunning) {
    result = cursor;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 32-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, cursor);
    StoreU32(ctx, frame + 8u, func_state);
    StoreU32(ctx, frame + 12u, scratch);
    StoreU32(ctx, frame + 16u, work);
    StoreU32(ctx, frame + 20u, value);
    StoreU32(ctx, frame + 24u, total);
    StoreU32(ctx, frame + 28u, extra);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
  return result;
}
}  // namespace artc
