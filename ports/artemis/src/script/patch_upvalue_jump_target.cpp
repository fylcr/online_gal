#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void EmitJumpInstruction(EngineContext* ctx, uint32_t state, uint32_t register_hint,
                         uint32_t target);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
uint32_t BindUpvalueCell(EngineContext* ctx, uint32_t state, uint32_t local, uint32_t upvalue);

namespace {

// One past the element count each of the prototype's arrays can hold: when a
// site would pass it, that site hands over to the too-big raise instead.
constexpr uint32_t kPatchUpvalueJumpTargetLineInfoLimit = 0x3FFFFFFFu;
constexpr uint32_t kPatchUpvalueJumpTargetCodeLimit = 0xFFFFFFFu;
constexpr uint32_t kPatchUpvalueJumpTargetConstantsLimit = 0xFFFFFFFu;
constexpr uint32_t kPatchUpvalueJumpTargetProtosLimit = 0x3FFFFFFFu;
constexpr uint32_t kPatchUpvalueJumpTargetLocVarsLimit = 0x15555555u;
constexpr uint32_t kPatchUpvalueJumpTargetSaveFrameBytes = 32u;
constexpr uint32_t kPatchUpvalueJumpTargetResumeWordBytes = 4u;

}  // namespace

// Ends the function the parser just finished: every local still open gets the
// pc its record ends at, the instruction the function closes with is emitted,
// the prototype's six arrays are shrunk to the counts the body actually used,
// the parser steps back to the function that enclosed it, and a name or
// string token the parse stopped on gets the cell it points at bound.
//
// The frame saves one word per slot below, so each slot keeps a single name
// across the whole walk even where a later site reuses it as a carry: `size`
// is the count a site grows to, then its byte size, then the base the
// reallocation answers with; `slot` is the array field a site writes, then the
// base it resizes, then the address of the upvalue's cell; `grow` is a site's
// fit check, then the base it resizes, then the byte count the upvalue array
// asks for; `mark` is the pc the closed locals end at, then a site's previous
// byte size; `proto` is the prototype the arrays belong to, then the parser
// state the upvalue step is handed.
void PatchUpvalueJumpTarget(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t size = 0u;
  uint32_t proto = 0u;
  uint32_t slot = 0u;
  uint32_t grow = 0u;
  uint32_t func_state = 0u;
  uint32_t holder = 0u;
  uint32_t resume_point = 0u;
  uint32_t mark = 0u;
  uint32_t call_result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 32-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kPatchUpvalueJumpTargetSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    proto = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
    grow = LoadU32(ctx, frame + 16u);
    func_state = LoadU32(ctx, frame + 20u);
    holder = LoadU32(ctx, frame + 24u);
    mark = LoadU32(ctx, frame + 28u);
    size = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kPatchUpvalueJumpTargetResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = LoadU32(ctx, state + 52u);
    func_state = LoadU32(ctx, state + 48u);
    proto = LoadU32(ctx, func_state);
    size = LoadU8(ctx, func_state + 50u);
    if (size != 0u) {
      // Close every local still open: each one's record ends at this pc.
      grow = LoadU32(ctx, proto + 24u);
      mark = LoadU32(ctx, func_state + 24u);
      do {
        size -= 1u;
        slot = size & 255u;
        StoreU32(ctx, grow + LoadU16(ctx, func_state + (slot << 1u) + 172u) * 12u + 8u, mark);
      } while (slot != 0u);
      StoreU8(ctx, func_state + 50u, 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // Emit the instruction the function closes with.
    EmitJumpInstruction(ctx, func_state, 0u, 0u);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    size = LoadU32(ctx, func_state + 24u);
    grow = static_cast<uint32_t>(size + 1u <= kPatchUpvalueJumpTargetLineInfoLimit);
    slot = proto;
  }
  if (grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Line info has no entry left for this pc.
    if (ResumePhase(ctx) == kResumeRunning) {
      grow = LoadU32(ctx, proto + 12u);
      mark = LoadU32(ctx, proto + 44u) << 2u;
      size = size << 2u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      call_result = ReallocateBlock(ctx, holder, grow, mark, size);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto lineinfo_ready; }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      call_result = RaiseBlockTooBig(ctx, holder);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
  }
lineinfo_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The line-info array holds every pc the body reached; the code array is
    // checked next.
    StoreU32(ctx, slot + 12u, size);
    StoreU32(ctx, proto + 44u, LoadU32(ctx, func_state + 24u));
    size = LoadU32(ctx, func_state + 24u);
    grow = static_cast<uint32_t>(size + 1u <= kPatchUpvalueJumpTargetCodeLimit);
    slot = proto;
  }
  if (grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The code array has no room left for one more instruction.
    if (ResumePhase(ctx) == kResumeRunning) {
      grow = LoadU32(ctx, proto + 20u);
      mark = LoadU32(ctx, proto + 48u) << 2u;
      size = size << 2u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      call_result = ReallocateBlock(ctx, holder, grow, mark, size);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto code_ready; }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      call_result = RaiseBlockTooBig(ctx, holder);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
  }
code_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The code array holds the whole body; the constants array is checked next.
    StoreU32(ctx, slot + 20u, size);
    StoreU32(ctx, proto + 48u, LoadU32(ctx, func_state + 24u));
    size = LoadU32(ctx, func_state + 40u);
    grow = static_cast<uint32_t>(size + 1u <= kPatchUpvalueJumpTargetConstantsLimit);
    slot = proto;
  }
  if (grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The constants array has no room left for one more entry.
    if (ResumePhase(ctx) == kResumeRunning) {
      grow = LoadU32(ctx, proto + 8u);
      mark = LoadU32(ctx, proto + 40u) << 4u;
      size = size << 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      call_result = ReallocateBlock(ctx, holder, grow, mark, size);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto constants_ready; }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      call_result = RaiseBlockTooBig(ctx, holder);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
  }
constants_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The constants array holds every constant the body used; the nested
    // prototypes are checked next.
    StoreU32(ctx, slot + 8u, size);
    StoreU32(ctx, proto + 40u, LoadU32(ctx, func_state + 40u));
    size = LoadU32(ctx, func_state + 44u);
    grow = static_cast<uint32_t>(size + 1u <= kPatchUpvalueJumpTargetProtosLimit);
    slot = proto;
  }
  if (grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The prototype array has no room left for one more nested prototype.
    if (ResumePhase(ctx) == kResumeRunning) {
      grow = LoadU32(ctx, proto + 16u);
      mark = LoadU32(ctx, proto + 52u) << 2u;
      size = size << 2u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      call_result = ReallocateBlock(ctx, holder, grow, mark, size);
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto protos_ready; }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      call_result = RaiseBlockTooBig(ctx, holder);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
  }
protos_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The prototype array holds every nested prototype; the local records are
    // checked next.
    StoreU32(ctx, slot + 16u, size);
    StoreU32(ctx, proto + 52u, LoadU32(ctx, func_state + 44u));
    size = LoadS16(ctx, func_state + 48u);
    grow = static_cast<uint32_t>(size + 1u <= kPatchUpvalueJumpTargetLocVarsLimit);
    slot = proto;
  }
  if (grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The local record array has no room left for one more record.
    if (ResumePhase(ctx) == kResumeRunning) {
      grow = LoadU32(ctx, proto + 24u);
      mark = LoadU32(ctx, proto + 56u) * 12u;
      size = size * 12u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      call_result = ReallocateBlock(ctx, holder, grow, mark, size);
      resume_point = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto locvars_ready; }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
      call_result = RaiseBlockTooBig(ctx, holder);
      resume_point = 10u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      size = call_result;
    }
  }
locvars_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The local records are sized; the upvalue array is resized outright, so
    // it needs no fit check.
    StoreU32(ctx, slot + 24u, size);
    StoreU32(ctx, proto + 56u, LoadS16(ctx, func_state + 48u));
    grow = LoadU8(ctx, proto + 72u) << 2u;
    slot = LoadU32(ctx, proto + 28u);
    size = LoadU32(ctx, proto + 36u) << 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    // The upvalue array is shrunk to the count of upvalues the body bound.
    call_result = ReallocateBlock(ctx, holder, slot, size, grow);
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    slot = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, proto + 28u, slot);
    slot = LoadU8(ctx, proto + 72u);
    StoreU32(ctx, proto + 36u, slot);
    StoreU32(ctx, state + 48u, LoadU32(ctx, func_state + 8u));
    proto = static_cast<uint32_t>(LoadU32(ctx, state + 16u) - 285u <= 1u);
  }
  if (proto != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The name or string token the parse stopped on carries a string: hand
      // the parser state, the string's data and its length to the binder.
      proto = state;
      state = LoadU32(ctx, state + 24u);
      slot = state + 16u;
      state = LoadU32(ctx, state + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
      BindUpvalueCell(ctx, proto, slot, state);
      resume_point = 12u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give back the 32-byte scratch this step was handed.
    StoreU32(ctx, holder + 8u, LoadU32(ctx, holder + 8u) - 32u);
  }
  goto exit_path;
suspend_point:
  // Suspend: record the resume point, then push the frame with the live words.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kPatchUpvalueJumpTargetResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, size);
    StoreU32(ctx, frame + 8u, proto);
    StoreU32(ctx, frame + 12u, slot);
    StoreU32(ctx, frame + 16u, grow);
    StoreU32(ctx, frame + 20u, func_state);
    StoreU32(ctx, frame + 24u, holder);
    StoreU32(ctx, frame + 28u, mark);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kPatchUpvalueJumpTargetSaveFrameBytes);
exit_path:
  LeaveFrame();
}
}  // namespace artc
