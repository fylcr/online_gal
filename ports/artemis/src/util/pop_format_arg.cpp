#include "util/engine_context.h"

namespace artc {

namespace {

constexpr uint32_t kPopVarArgConstFFFFFFF8 = 0xFFFFFFF8u;
constexpr uint32_t kPopVarArgSaveFrameBytes = 12u;
constexpr uint32_t kPopVarArgResumeWordBytes = 4u;

}  // namespace

// Pops one formatter argument off the cursor at `ap` into the result slot at
// `out`: the type code picks the width read, its signedness and how the value
// widens into the 64-bit slot. Answers nothing.
void PopVarArg(EngineContext* ctx, uint32_t out, uint32_t type_code, uint32_t ap,
               uint32_t scratch) {
  EnterFrame();
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kPopVarArgSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    out = LoadU32(ctx, frame);
    ap = LoadU32(ctx, frame + 4u);
    scratch = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kPopVarArgResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (type_code > 20u) {
      // Past the widest arm: nothing to pop and no call to replay.
      goto pop_done;
    }
    switch (type_code - 9u) {
      case 0: goto pop_word;
      case 1: goto pop_signed_word;
      case 2: goto pop_unsigned_word;
      case 3: goto pop_aligned_word;
      case 4: goto pop_signed_half;
      case 5: goto pop_unsigned_half;
      case 6: goto pop_signed_byte;
      case 7: goto pop_unsigned_byte;
      case 8: goto pop_aligned_double;
      case 9: goto pop_replay;
      default: goto pop_done;
    }
    // One arm per width: the cursor word is the value's address, and it steps
    // past the bytes it just handed over.
    pop_word:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint32_t value = LoadU32(ctx, slot);
      StoreU32(ctx, out, value);
      goto pop_done;
    }
    pop_signed_word:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint64_t value = static_cast<uint64_t>(
          static_cast<int64_t>(static_cast<int32_t>(LoadU32(ctx, slot))));
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_unsigned_word:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint64_t value = static_cast<uint64_t>(LoadU32(ctx, slot));
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_aligned_word:;
    {
      // The 64-bit slot is aligned up to eight bytes before it is read.
      uint32_t slot = (LoadU32(ctx, ap) + 7u) & kPopVarArgConstFFFFFFF8;
      StoreU32(ctx, ap, slot + 8u);
      uint64_t value = LoadU64(ctx, slot);
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_signed_half:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint64_t value = static_cast<uint64_t>(static_cast<int64_t>(LoadS16(ctx, slot)));
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_unsigned_half:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint64_t value = static_cast<uint64_t>(LoadU16(ctx, slot));
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_signed_byte:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint64_t value = static_cast<uint64_t>(static_cast<int64_t>(LoadS8(ctx, slot)));
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_unsigned_byte:;
    {
      uint32_t slot = LoadU32(ctx, ap);
      StoreU32(ctx, ap, slot + 4u);
      uint64_t value = static_cast<uint64_t>(LoadU8(ctx, slot));
      StoreU64(ctx, out, value);
      goto pop_done;
    }
    pop_aligned_double:;
    {
      // As above, the double slot is aligned up to eight bytes first.
      uint32_t slot = (LoadU32(ctx, ap) + 7u) & kPopVarArgConstFFFFFFF8;
      StoreU32(ctx, ap, slot + 8u);
      double value = LoadF64(ctx, slot);
      StoreF64(ctx, out, value);
      goto pop_done;
    }
    // This width has no arm of its own: it falls out of the gate to the replay.
    pop_replay:;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
    // The unmatched width is handed to the table slot the formatter dispatches.
    CallTableT0(ctx, scratch, out, ap);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  pop_done:;
  LeaveFrame();
  return;
suspend_here:;
  // Push the resume word naming the call in flight, then the frame the rewind
  // pops; the words go in the order the pop reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kPopVarArgResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, out);
  StoreU32(ctx, base + 4u, ap);
  StoreU32(ctx, base + 8u, scratch);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kPopVarArgSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
