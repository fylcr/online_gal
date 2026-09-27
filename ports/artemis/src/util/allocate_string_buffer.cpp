// allocate_string_buffer.cpp — the engine's suspendable heap allocation entry.
//
// AllocateStringBuffer hands out guest-heap bytes. It asks the raw heap
// allocator for the block first and returns that pointer untouched; a refusal
// is not the end of the request, because the engine registers a collector step
// in its function table and this body runs one step and asks again. Only a heap
// that still refuses with no collector step registered turns the failure into
// the engine's out-of-memory exception.
//
// The collector step is real guest code and can suspend, so this body keeps the
// asyncify driver's shape: a rewinding pass pops the 8-byte frame holding the
// requested size and the in-flight slot, then the word naming the call that was
// in flight, and exactly that one call is replayed. The push order is the
// engine's own — the word first, then the frame — and the two rewinding pops
// take them back in reverse, one pop per saved region.
//
// A fresh request of zero bytes is served as one byte, and only on a running
// pass: a rewinding pass keeps the size its frame restored. Judgment order is
// the engine's own throughout. The size fixup runs before the retry loop; the
// raw allocation attempt runs on a running pass only; the collector gate ors
// the rewinding bit into the slot rather than testing the slot alone; and the
// replay guard asks the resume word before the step is run again. Merging the
// two rewinding pops, or folding a phase gate into the test beside it, would
// change that order rather than merely read nicer.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// The raw heap allocator, the collector step's table dispatch and the host
// exception pair are all named in util/engine_context.h, so this body calls
// them directly and declares none of them again.

namespace {

// The collector step's table index lives in this guest word, and the allocator
// reads it only after the raw heap has already refused a request.
constexpr uint32_t kCollectorStepIndexAddr = 856940u;
// The out-of-memory failure leaves as the engine's bad_alloc-shaped exception:
// a four-byte record is allocated from the host, its leading word is stamped
// with the two type words the throw pair carries （the second stamp is the
// surviving one; the C++ typeinfo names this area holds are in
// code/strings.tsv around 798931）, and the throw takes the record, the
// typeinfo word and the destructor word.
constexpr uint32_t kThrowRecordBytes = 4u;
constexpr uint32_t kThrowTypeWordFirst = 798992u;
constexpr uint32_t kThrowTypeWordSecond = 798956u;
constexpr uint32_t kThrowTypeInfoAddr = 799044u;
constexpr uint32_t kThrowDestructorWord = 2270u;
// The one await site in this body names itself zero, which the driver tests
// with a bare zero rather than a compare.
constexpr uint32_t kCollectorStepCall = 0u;

}  // namespace

uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t bytes) {
  EnterFrame();
  // The body's reused slots: the slot that carries either the raw allocation
  // result or the collector step's table index, the word naming the call in
  // flight, the temporary a phase gate is folded into, and the value handed
  // back to the caller. All four are declared up here because the labels below
  // are jumped over, jump past their own assignment points, and because a
  // rewinding pass restores the first two from the saved frame.
  uint32_t slot = 0u;
  uint32_t resume_word = 0u;
  uint32_t gate = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path pushed and rebuild the
    // request size and the slot from it — the size first, then the slot.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    bytes = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the word naming the call that was in flight. This pop
    // stays separate from the frame pop above — the driver keeps them one
    // region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // Size fixup: a fresh zero-byte request is served as one byte, and only on a
  // running pass — a rewinding pass keeps the size its frame restored.
  bytes = ResumePhase(ctx) ? bytes : (bytes ? bytes : 1u);
 retry_alloc:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the raw heap is asked first. A pointer answers the request; a
    // zero refusal is what sends the body looking for the collector step's
    // table index.
    slot = AllocateRawBlock(ctx, bytes);
    if (slot) {
      goto alloc_done;
    }
    slot = LoadU32(ctx, kCollectorStepIndexAddr);
  }
  // The step is attempted when there is a step to attempt or the pass is
  // rewinding; the gate ors the rewinding bit into the slot rather than asking
  // the slot on its own.
  gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate) {
    // A running pass always runs the step; a rewinding pass replays it only
    // when the word naming the call in flight is the one this body pushes —
    // the bare-zero test the driver keeps at this site.
    if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
      CallCollectorStep(ctx, slot);
      resume_word = kCollectorStepCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The step returned without suspending: the heap has not been asked
      // since the refusal, so the request is put to it again.
      goto retry_alloc;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running and out of road: the heap refused and no collector step is
    // registered, so the failure leaves as the out-of-memory exception.
    slot = HostAllocateException(ctx, kThrowRecordBytes);
    StoreU32(ctx, slot, kThrowTypeWordFirst);
    StoreU32(ctx, slot, kThrowTypeWordSecond);
    HostThrowException(ctx, slot, kThrowTypeInfoAddr, kThrowDestructorWord);
    TrapUnreachable();
  }
 alloc_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the slot holds the allocated block and it is the answer.
    result = slot;
    goto exit_path;
  }
  TrapUnreachable();
 suspend_point:;
  {
    // Suspend: push the word naming the call in flight, step past it, then push
    // the 8-byte frame a rewinding pass reads back — the requested size and the
    // slot. The stack pointer lives in the word the frame pointer itself names,
    // so every push is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_word);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, bytes);
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  }
  result = 0u;
 exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
