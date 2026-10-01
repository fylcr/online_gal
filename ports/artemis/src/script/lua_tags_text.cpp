#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

// The `print` literal the step copies into the string header it builds.
constexpr uint32_t kLuaTagsTextPrintTextAddr = 0xB94DDu;
// The byte right after that literal, which ends the header run.
constexpr uint32_t kLuaTagsTextPrintTailAddr = 0xB94E1u;
// The allocator diagnostic reported when a copy would overflow its element count.
constexpr uint32_t kLuaTagsTextAllocatorErrorAddr = 0xB962Fu;
// The pending-exception latch the invoked calls report through.
constexpr uint32_t kLuaTagsTextThrowLatchAddr = 0xD015Cu;
// The `data` tag of the buffer the string header points at.
constexpr uint32_t kLuaTagsTextDataTag = 0x61746164u;
// The owner count of a released block.
constexpr uint32_t kLuaTagsTextNoOwner = 0xFFFFFFFFu;
// The element counts a copy may still hold: signed, unsigned, and the allocation ceiling.
constexpr uint32_t kLuaTagsTextMaxSignedCount = 0x1FFFFFFFu;
constexpr uint32_t kLuaTagsTextMaxUnsignedCount = 0x3FFFFFFFu;
constexpr uint32_t kLuaTagsTextMaxAllocCount = 0x40000000u;

}  // namespace

namespace MessageLayer {

// Appends a text run to the print queue of the message layer: it scans the run for the first
// blank, builds the `print` string header on a 48-byte guest scratch frame, grows the layer's
// string buffer when the run does not fit and copies the run into it. Every call that can run
// guest code is a suspend gate: a rewinding pass restores the frame slots below and replays the
// one call its resume code names. `a` is the message layer and `b` the text run -- that slot
// also carries the write cursor and the other temporaries the flattened driver keeps in it.
void QueuePrintText(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 48u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the calls below spill, in the order they run: the two buffer allocations, the
  // three invokes, the slot releases of the two copy paths and the layout folds that shift the
  // buffer offsets.
  uint32_t const kAllocateBufferCall = 0u;
  uint32_t const kInvokePrintCall = 1u;
  uint32_t const kReleaseSlotCall = 2u;
  uint32_t const kReleaseAllocatorSlotCall = 3u;
  uint32_t const kFoldLayoutCall = 4u;
  uint32_t const kAllocateCopyCall = 5u;
  uint32_t const kInvokeCopyCall = 6u;
  uint32_t const kInvokeSecondPrintCall = 7u;
  uint32_t const kReleaseSecondSlotCall = 8u;
  uint32_t const kReleaseSecondAllocatorCall = 9u;
  uint32_t const kFoldSecondLayoutCall = 10u;
  // The frame saves one word per slot below, so each slot keeps a single name across the whole
  // step even where a later region reuses it: the general temporary, the scan index, the result
  // of the call in flight, the 48-byte guest scratch frame, the flag the growth paths carry, the
  // element count, the old size, the scan cursor, the freshly allocated string buffer, the
  // resume code in flight, the buffer handle and the sign-extended length byte of the run.
  uint32_t scratch = 0u;
  uint32_t index = 0u;
  uint32_t call_result = 0u;
  uint32_t scratch_frame = 0u;
  uint32_t flag = 0u;
  uint32_t count = 0u;
  uint32_t size = 0u;
  uint32_t scan = 0u;
  uint32_t buffer = 0u;
  uint32_t resume_code = 0u;
  uint32_t handle = 0u;
  uint32_t byte_value = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 48-byte frame and restore every slot from it, in the order the
    // suspend path saved them.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, saved_frame);
    scratch = LoadU32(ctx, saved_frame + 8u);
    index = LoadU32(ctx, saved_frame + 12u);
    scratch_frame = LoadU32(ctx, saved_frame + 16u);
    flag = LoadU32(ctx, saved_frame + 20u);
    count = LoadU32(ctx, saved_frame + 24u);
    size = LoadU32(ctx, saved_frame + 28u);
    scan = LoadU32(ctx, saved_frame + 32u);
    buffer = LoadU32(ctx, saved_frame + 36u);
    handle = LoadU32(ctx, saved_frame + 40u);
    byte_value = LoadU32(ctx, saved_frame + 44u);
    b = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay separate -- the
    // driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the scratch frame, then read the run: its count, the byte at +11 and the first
    // blank (or non-blank) position the caller has to copy from.
    scratch_frame = GuestStackSp(ctx) - 48u;
    GuestStackSp(ctx) = scratch_frame;
    count = LoadU32(ctx, b + 4u);
    size = LoadU8(ctx, b + 11u);
    byte_value = static_cast<uint32_t>(static_cast<int32_t>(size << 24u) >> 24);
    if (static_cast<int32_t>(byte_value) >= 0) {
      scratch = size;
      scan = static_cast<uint32_t>(byte_value == 0u);
      if (scan != 0u) {
        goto finished;
      }
      for (;;) {
        scan = static_cast<uint32_t>(LoadU8(ctx, b + index) != 32u);
        if (scan != 0u) {
          scratch = index;
          goto finished;
        }
        index = index + 1u;
        scan = static_cast<uint32_t>(size != index);
        if (scan == 0u) {
          break;
        }
      }
      goto finished;
    }
    scratch = count;
    index = static_cast<uint32_t>(count == 0u);
    if (index != 0u) {
      goto finished;
    }
    index = LoadU32(ctx, b);
    scratch = 0u;
    for (;;) {
      scan = static_cast<uint32_t>(LoadU8(ctx, index + scratch) != 32u);
      if (scan != 0u) {
        goto finished;
      }
      scratch = scratch + 1u;
      scan = static_cast<uint32_t>(count != scratch);
      if (scan == 0u) {
        break;
      }
    }
    scratch = count;
  finished:;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A run that still has text in front of it: grow the layer's string buffer if it is full,
    // otherwise remember how much of it is free.
    if (scratch != 0u) {
      scan = LoadU32(ctx, a + 196u);
      handle = LoadU32(ctx, a + 8u);
      index = static_cast<uint32_t>(scan >= handle);
      if (index != 0u) {
        goto cleanup_done;
      }
      buffer = LoadU8(ctx, a + 89u);
      index = scan;
      for (;;) {
        flag = LoadU8(ctx, index);
        if (buffer == flag) {
          flag = static_cast<uint32_t>(LoadU8(ctx, index + 1u) != 32u);
          if (flag != 0u) {
            break;
          }
          uint32_t const probe = index + 1u;
          uint32_t const complement = scan ^ kLuaTagsTextNoOwner;
          index = complement;
          scratch = probe + complement;
          break;
        }
        flag = static_cast<uint32_t>(flag != 32u);
        if (flag != 0u) {
          break;
        }
        index = index + 1u;
        flag = static_cast<uint32_t>(handle > index);
        if (flag == 0u) {
          break;
        }
      }
      if (scratch != 0u) {
        goto cleanup_done;
      }
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAllocateBufferCall) : 1u) {
    // Allocate the 44-byte string header the run is appended to.
    call_result = AllocateStringBuffer(ctx, 44u);
    resume_code = kAllocateBufferCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    buffer = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Build the header on the scratch frame: its tag byte, the `print` literal, its trailing
    // byte, the empty `data` buffer and the layer's own exception word.
    StoreU8(ctx, scratch_frame + 43u, 5u);
    StoreU32(ctx, scratch_frame + 32u, LoadU32(ctx, kLuaTagsTextPrintTextAddr));
    StoreU8(ctx, scratch_frame + 36u, LoadU8(ctx, kLuaTagsTextPrintTailAddr));
    StoreU8(ctx, scratch_frame + 37u, 0u);
    StoreU8(ctx, scratch_frame + 20u, 0u);
    StoreU32(ctx, scratch_frame + 16u, kLuaTagsTextDataTag);
    StoreU8(ctx, scratch_frame + 27u, 4u);
    scratch = LoadU32(ctx, a + 16u);
    StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
    count = scratch_frame + 16u;
    index = scratch_frame + 32u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInvokePrintCall) : 1u) {
    // Append the run to the `print` string through the invoked helper.
    call_result = HostInvokeIiiiii(ctx, 3195u, buffer, index, count, b, scratch);
    resume_code = kInvokePrintCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    b = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The helper reports through the latch: an error skips to the release path, otherwise the
    // run has to be written into the layer's buffer.
    b = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
    StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
    flag = 1u;
    if (b == 1u) {
      goto slot_released;
    }
    b = LoadU32(ctx, a + 96u);
    scratch = LoadU32(ctx, a + 100u);
    index = static_cast<uint32_t>(b < scratch);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (index != 0u) {
      // Room for one more: write the buffer pointer and advance the layer's cursor.
      StoreU32(ctx, b, buffer);
      b = b + 4u;
      StoreU32(ctx, a + 96u, b);
      goto append_done;
    }
    // Grow the buffer: half the room left, capped by the element limits.
    uint32_t const write_pos = b;
    b = a + 92u;
    size = LoadU32(ctx, b);
    scan = write_pos - size;
    count = scan >> 2u;
    index = count + 1u;
    flag = static_cast<uint32_t>(index >= kLuaTagsTextMaxAllocCount);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      flag = 0u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReleaseSlotCall) : 1u) {
      HostReleaseSlotPair(ctx, 1053u, b);
      resume_code = kReleaseSlotCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      if (a == 1u) {
        goto slot_released;
      }
      goto cleanup_done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Clamp the requested size: half the room left, at most the element limit of a signed count
    // and failing that the limit of an unsigned one.
    scratch = scratch - size;
    b = scratch >> 1u;
    flag = static_cast<uint32_t>(index > b);
    uint32_t const selected = flag != 0u ? index : b;
    scratch = static_cast<uint32_t>((scratch >> 2u) < kLuaTagsTextMaxSignedCount);
    index = scratch != 0u ? selected : kLuaTagsTextMaxUnsignedCount;
    b = static_cast<uint32_t>(index == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (b != 0u) {
      scratch = 0u;
      goto grow_done;
    }
    b = static_cast<uint32_t>(index >= kLuaTagsTextMaxAllocCount);
  }
  if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      flag = 0u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReleaseAllocatorSlotCall) : 1u) {
      HostReleaseSlotPair(ctx, 1033u, kLuaTagsTextAllocatorErrorAddr);
      resume_code = kReleaseAllocatorSlotCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      if (a != 1u) {
        goto cleanup_done;
      }
      goto slot_released;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = 0u;
    StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
    b = index << 2u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFoldLayoutCall) : 1u) {
    // Shift the buffer offset by the layout of the entry that was just reserved.
    call_result = HostFoldLayoutOffset(ctx, 1032u, b);
    resume_code = kFoldLayoutCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    scratch = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
    StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
    if (b == 1u) {
      goto slot_released;
    }
  }
  grow_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Lay the run out in the (possibly fresh) buffer and publish the new sizes.
    b = scratch + (count << 2u);
    StoreU32(ctx, b, buffer);
    count = static_cast<uint32_t>(index >= 1u);
    if (count != 0u) {
      count = CopyBytes(ctx, scratch, size, index);
    }
    index = (index << 2u) + scratch;
    StoreU32(ctx, a + 100u, index);
    b = b + 4u;
    StoreU32(ctx, a + 96u, b);
    StoreU32(ctx, a + 92u, scratch);
    a = static_cast<uint32_t>(size == 0u);
    if (a != 0u) {
      goto append_done;
    }
    DeallocateRawBlock(ctx, size);
  }
  append_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Release the string header built on the scratch frame: its data buffer first and then the
    // `print` buffer, unless the trailing byte still belongs to this frame.
    if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 27u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 16u));
    }
    if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 43u)) > static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      goto release_scratch;
    }
    DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 32u));
    goto release_scratch;
  }
  cleanup_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Nothing left to copy once the run is no longer than what the buffer already holds.
    uint32_t const pending = static_cast<int32_t>(byte_value) < 0 ? count : size;
    index = static_cast<uint32_t>(pending <= scratch);
    if (index != 0u) {
      goto release_scratch;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAllocateCopyCall) : 1u) {
    // Allocate the second string header, the one the run is copied into.
    call_result = AllocateStringBuffer(ctx, 44u);
    resume_code = kAllocateCopyCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    buffer = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Build the header again, this time for the grown buffer.
    StoreU8(ctx, scratch_frame + 43u, 5u);
    StoreU32(ctx, scratch_frame + 32u, LoadU32(ctx, kLuaTagsTextPrintTextAddr));
    index = LoadU8(ctx, kLuaTagsTextPrintTailAddr);
    StoreU8(ctx, scratch_frame + 36u, index);
    StoreU8(ctx, scratch_frame + 37u, 0u);
    StoreU8(ctx, scratch_frame + 20u, 0u);
    StoreU32(ctx, scratch_frame + 16u, kLuaTagsTextDataTag);
    StoreU8(ctx, scratch_frame + 27u, 4u);
    StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInvokeCopyCall) : 1u) {
    // Copy the old run into the new header and hand it back.
    call_result = HostInvokeIiiiii(ctx, 854u, scratch_frame, b, scratch, kLuaTagsTextNoOwner, b);
    resume_code = kInvokeCopyCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    handle = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
    StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
    flag = 1u;
    b = static_cast<uint32_t>(b != 1u);
  }
  if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      b = LoadU32(ctx, a + 16u);
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      index = scratch_frame + 16u;
      scratch = scratch_frame + 32u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInvokeSecondPrintCall) : 1u) {
      call_result = HostInvokeIiiiii(ctx, 3195u, buffer, scratch, index, handle, b);
      resume_code = kInvokeSecondPrintCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      b = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      b = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      if (b == 1u) {
        goto slot_released_second;
      }
      b = LoadU32(ctx, a + 96u);
      scratch = LoadU32(ctx, a + 100u);
      index = static_cast<uint32_t>(b < scratch);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (index != 0u) {
        StoreU32(ctx, b, buffer);
        b = b + 4u;
        StoreU32(ctx, a + 96u, b);
        goto append_copy_done;
      }
      uint32_t const write_pos = b;
      b = a + 92u;
      size = LoadU32(ctx, b);
      scan = write_pos - size;
      count = scan >> 2u;
      index = count + 1u;
      flag = static_cast<uint32_t>(index >= kLuaTagsTextMaxAllocCount);
    }
    if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
        flag = 0u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReleaseSecondSlotCall) : 1u) {
        HostReleaseSlotPair(ctx, 1053u, b);
        resume_code = kReleaseSecondSlotCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        a = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
        StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
        if (a != 1u) {
          goto trap_path;
        }
        goto slot_released_second;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = scratch - size;
      b = scratch >> 1u;
      flag = static_cast<uint32_t>(index > b);
      uint32_t const selected = flag != 0u ? index : b;
      scratch = static_cast<uint32_t>((scratch >> 2u) < kLuaTagsTextMaxSignedCount);
      index = scratch != 0u ? selected : kLuaTagsTextMaxUnsignedCount;
      b = static_cast<uint32_t>(index == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (b != 0u) {
        scratch = 0u;
        goto grow_copy_done;
      }
      b = static_cast<uint32_t>(index >= kLuaTagsTextMaxAllocCount);
    }
    if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
        flag = 0u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReleaseSecondAllocatorCall) : 1u) {
        HostReleaseSlotPair(ctx, 1033u, kLuaTagsTextAllocatorErrorAddr);
        resume_code = kReleaseSecondAllocatorCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        a = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
        StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
        if (a != 1u) {
          goto trap_path;
        }
        goto slot_released_second;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = 0u;
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      b = index << 2u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFoldSecondLayoutCall) : 1u) {
      call_result = HostFoldLayoutOffset(ctx, 1032u, b);
      resume_code = kFoldSecondLayoutCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      scratch = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      b = LoadU32(ctx, kLuaTagsTextThrowLatchAddr);
      StoreU32(ctx, kLuaTagsTextThrowLatchAddr, 0u);
      if (b == 1u) {
        goto slot_released_second;
      }
    }
    grow_copy_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Copy the run into the grown buffer and publish the shifted sizes.
      b = scratch + (count << 2u);
      StoreU32(ctx, b, buffer);
      if (static_cast<int32_t>(index) >= 1) {
        (void)CopyBytes(ctx, scratch, size, index);
      }
      index = (index << 2u) + scratch;
      StoreU32(ctx, a + 100u, index);
      b = b + 4u;
      StoreU32(ctx, a + 96u, b);
      StoreU32(ctx, a + 92u, scratch);
      a = static_cast<uint32_t>(size == 0u);
      if (a != 0u) {
        goto append_copy_done;
      }
      DeallocateRawBlock(ctx, size);
    }
    append_copy_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Release the header of the grown buffer and the two buffers of the scratch frame.
      if (static_cast<int32_t>(LoadS8(ctx, handle + 11u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
        DeallocateRawBlock(ctx, LoadU32(ctx, handle));
      }
      if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 27u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
        DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 16u));
      }
      if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 43u)) > static_cast<int32_t>(kLuaTagsTextNoOwner)) {
        goto release_scratch;
      }
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 32u));
      goto release_scratch;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    goto os_release_path;
  }
  slot_released_second:;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, handle + 11u)) > static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      goto os_release_path;
    }
    DeallocateRawBlock(ctx, LoadU32(ctx, handle));
  }
  os_release_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Drop what the scratch frame still owns and hand the buffer back to the host.
    if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 27u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 16u));
    }
    if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 43u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 32u));
    }
    if (flag == 0u) {
      goto os_release_done;
    }
    DeallocateRawBlock(ctx, buffer);
    goto os_release_done;
  }
  release_scratch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch_frame = scratch_frame + 48u;
    GuestStackSp(ctx) = scratch_frame;
    goto exit_path;
  }
  slot_released:;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 27u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 16u));
    }
    if (static_cast<int32_t>(LoadS8(ctx, scratch_frame + 43u)) <= static_cast<int32_t>(kLuaTagsTextNoOwner)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_frame + 32u));
    }
    if (flag == 0u) {
      goto os_release_done;
    }
    DeallocateRawBlock(ctx, buffer);
    HostOsRelease(ctx, scratch);
    TrapUnreachable();
  }
  os_release_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, scratch);
    TrapUnreachable();
  }
  trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: store the code of the call in flight, step past it, then push the 48-byte frame
    // a rewinding pass reads back -- one word per slot, in the order they were saved.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, a);
    StoreU32(ctx, saved_frame + 4u, b);
    StoreU32(ctx, saved_frame + 8u, scratch);
    StoreU32(ctx, saved_frame + 12u, index);
    StoreU32(ctx, saved_frame + 16u, scratch_frame);
    StoreU32(ctx, saved_frame + 20u, flag);
    StoreU32(ctx, saved_frame + 24u, count);
    StoreU32(ctx, saved_frame + 28u, size);
    StoreU32(ctx, saved_frame + 32u, scan);
    StoreU32(ctx, saved_frame + 36u, buffer);
    StoreU32(ctx, saved_frame + 40u, handle);
    StoreU32(ctx, saved_frame + 44u, byte_value);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace MessageLayer

}  // namespace artc
