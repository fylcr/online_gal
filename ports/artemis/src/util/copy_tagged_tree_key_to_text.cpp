#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {
// The word a pending result is published in.
constexpr uint32_t kCopyTaggedTreeKeyToTextStatusAddr = 0xD015Cu;
// The all-bits word a signed comparison result is held against.
constexpr uint32_t kCopyTaggedTreeKeyToTextAllBits = 0xFFFFFFFFu;
constexpr uint32_t kCopyTaggedTreeKeyToTextSaveFrameBytes = 32u;
constexpr uint32_t kCopyTaggedTreeKeyToTextResumeWordBytes = 4u;
}  // namespace

// Looks the entry up in a tagged tree and assigns the matched entry's key into
// the destination string, in either the short or the long string form; a fresh
// entry is inserted and linked in when the lookup finds none.
void CopyTaggedTreeKeyToText(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c,
                             uint32_t d, uint32_t e, uint32_t f) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  uint32_t fifth = 0u;
  // The frame base, a comparison result, and the new buffer.
  uint32_t hold = 0u;
  // The entry key's length, and later the destination block.
  uint32_t key_len = 0u;
  // A key pointer, a flag, and the slot the OsQuery fills.
  uint32_t slot = 0u;
  // The await code a suspend was resumed with.
  uint32_t await_code = 0u;
  // The key the entry is looked up with.
  uint32_t key_ptr = 0u;
  // Set when the entry key is longer than the node's.
  uint32_t longer = 0u;
  // The length the two keys are compared over.
  uint32_t cmp_len = 0u;
  // The value the insertion gate carries.
  uint32_t gate = 0u;
  uint64_t zero = 0;

  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCopyTaggedTreeKeyToTextSaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    hold = top;
    a = LoadU32(ctx, hold);
    c = LoadU32(ctx, hold + 8u);
    d = LoadU32(ctx, hold + 12u);
    e = LoadU32(ctx, hold + 16u);
    f = LoadU32(ctx, hold + 20u);
    key_len = LoadU32(ctx, hold + 24u);
    slot = LoadU32(ctx, hold + 28u);
    b = LoadU32(ctx, hold + 4u);
  }
  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCopyTaggedTreeKeyToTextResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    await_code = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    d = LoadU32(ctx, b + 4u);
    if (d != 0u) {
      // Walk the tree: hold the entry key against each node's key.
      top = LoadU32(ctx, c);
      arg = c;
      third = c;
      third = LoadU8(ctx, third + 11u);
      f = third;
      fourth = 24u;
      third = third << (fourth & 31);
      fourth = 24u;
      third = (uint32_t)((int32_t)third >> (fourth & 31));
      fourth = 0u;
      third = (uint32_t)((int32_t)third < (int32_t)fourth);
      key_len = third;
      top = third ? top : arg;
      key_ptr = top;
      top = LoadU32(ctx, c + 4u);
      arg = f;
      third = key_len;
      top = third ? top : arg;
      key_len = top;
      top = b;
      arg = 4u;
      top = top + arg;
      f = top;
      while (true) {
        top = LoadU8(ctx, d + 27u);
        c = top;
        arg = 24u;
        top = top << (arg & 31);
        arg = 24u;
        top = (uint32_t)((int32_t)top >> (arg & 31));
        arg = 0u;
        top = (uint32_t)((int32_t)top < (int32_t)arg);
        slot = top;
        top = key_len;
        arg = LoadU32(ctx, d + 20u);
        third = c;
        fourth = slot;
        arg = fourth ? arg : third;
        c = arg;
        top = top > arg;
        longer = top;
        top = c;
        arg = key_len;
        third = longer;
        top = third ? top : arg;
        cmp_len = top;
        if (top != 0u) {
          top = key_ptr;
          arg = d;
          third = 16u;
          arg = arg + third;
          hold = arg;
          arg = LoadU32(ctx, arg);
          third = hold;
          fourth = slot;
          arg = fourth ? arg : third;
          slot = arg;
          third = cmp_len;
          top = CompareBytes(ctx, top, arg, third);
          hold = top;
          if (top == 0u) {
            top = c;
            arg = key_len;
            top = top > arg;
            if (top != 0u) {
              goto descend_first;
            }
            goto compare_next;
          }
          top = hold;
          arg = kCopyTaggedTreeKeyToTextAllBits;
          top = (uint32_t)((int32_t)top > (int32_t)arg);
          if (top != 0u) {
            goto compare_next;
          }
          goto descend_first;
        }
        top = c;
        arg = key_len;
        top = top <= arg;
        if (top != 0u) {
          goto check_direction;
        }
      descend_first:;
        c = LoadU32(ctx, d);
        if (c != 0u) {
          goto advance;
        }
        goto stop_here;
      compare_next:;
        top = slot;
        arg = key_ptr;
        third = cmp_len;
        top = CompareBytes(ctx, top, arg, third);
        c = top;
        if (top != 0u) {
          goto check_result;
        }
      check_direction:;
        top = longer;
        if (top != 0u) {
          goto descend_second;
        }
        goto take_key;
      check_result:;
        top = c;
        arg = kCopyTaggedTreeKeyToTextAllBits;
        top = (uint32_t)((int32_t)top > (int32_t)arg);
        if (top != 0u) {
          goto take_key;
        }
      descend_second:;
        f = d;
        arg = 4u;
        f = f + arg;
        c = LoadU32(ctx, d + 4u);
        slot = c == 0u;
        if (slot != 0u) {
          goto take_key;
        }
        d = f;
      advance:;
        f = d;
        d = c;
        continue;
      }
      TrapUnreachable();
    }
    d = b;
    arg = 4u;
    d = d + arg;
  stop_here:;
    f = d;
  take_key:;
    key_len = a;
    c = LoadU32(ctx, f);
  }
  top = b;
  arg = 0u;
  third = ResumePhase(ctx);
  top = third ? top : arg;
  arg = b;
  third = gate;
  fourth = c;
  fifth = ResumePhase(ctx);
  third = fifth ? third : fourth;
  gate = third;
  fourth = ResumePhase(ctx);
  fifth = 2u;
  fourth = fourth == fifth;
  third = third | fourth;
  top = third ? top : arg;
  b = top;
  if (gate == 0u || ResumePhase(ctx) == 2u) {
    if (await_code == 0u || ResumePhase(ctx) == kResumeRunning) {
      hold = AllocateStringBuffer(ctx, 32u);
      top = 0u;
      arg = ResumePhase(ctx);
      arg = arg == kResumeSuspended;
      if (arg != 0u) {
        goto save_frame;
      }
      c = hold;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      e = LoadU32(ctx, e);
      StoreU32(ctx, kCopyTaggedTreeKeyToTextStatusAddr, 0u);
      slot = c + 16u;
    }
    if (await_code == 1u || ResumePhase(ctx) == kResumeRunning) {
      HostOsQuery(ctx, 873u, slot, e);
      top = 1u;
      arg = ResumePhase(ctx);
      arg = arg == kResumeSuspended;
      if (arg != 0u) {
        goto save_frame;
      }
    }
    // The short string form writes the key straight into the buffer.
    if (ResumePhase(ctx) != 0u) {
      top = b;
    } else {
      e = LoadU32(ctx, kCopyTaggedTreeKeyToTextStatusAddr);
      StoreU32(ctx, kCopyTaggedTreeKeyToTextStatusAddr, 0u);
      if (e == 1u) {
        goto release_and_trap;
      }
      StoreU32(ctx, c + 8u, d);
      zero = 0ull;
      StoreU64(ctx, c, zero);
      StoreU32(ctx, c + 28u, 0u);
      StoreU32(ctx, f, c);
      e = c;
      top = LoadU32(ctx, b);
      d = LoadU32(ctx, top);
      if (d != 0u) {
        StoreU32(ctx, b, d);
        e = LoadU32(ctx, f);
      }
      top = LoadU32(ctx, b + 4u);
      arg = e;
      UnlinkListNode(ctx, top, arg);
      top = LoadU32(ctx, b + 8u);
      arg = 1u;
      top = top + arg;
      StoreU32(ctx, b + 8u, top);
      top = 1u;
    }
    b = top;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, key_len + 4u, b);
    StoreU32(ctx, a, c);
    LeaveFrame();
    return;
  }
release_and_trap:;
  // A pending result was raised, so release the buffer and trap.
  if (ResumePhase(ctx) == kResumeRunning) {
    a = HostRet0(ctx);
    top = HostTempRet0(ctx);
    top = c;
    DeallocateRawBlock(ctx, top);
    top = a;
    HostOsRelease(ctx, top);
    TrapUnreachable();
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  hold = top;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, hold);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kCopyTaggedTreeKeyToTextResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  hold = top;
  StoreU32(ctx, hold, a);
  StoreU32(ctx, hold + 4u, b);
  StoreU32(ctx, hold + 8u, c);
  StoreU32(ctx, hold + 12u, d);
  StoreU32(ctx, hold + 16u, e);
  StoreU32(ctx, hold + 20u, f);
  StoreU32(ctx, hold + 24u, key_len);
  StoreU32(ctx, hold + 28u, slot);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + 32u;
  StoreU32(ctx, top, arg);
  LeaveFrame();
}

}  // namespace artc
