#include "util/engine_context.h"

namespace artc {

// Reads a script string as a number, keeping its text for the error it may raise.
uint32_t NumberToString(EngineContext* ctx, uint32_t value, uint32_t options);
// Looks up the slot a numeric key already holds in the frame's key table.
uint32_t FindNumericKeySlot(EngineContext* ctx, uint32_t state);

namespace {
// The variable table's first entry, and the address a variable without a slot
// resolves to.
constexpr uint32_t kResolveVariableSlotEntryBase = 0x27118u;
constexpr uint32_t kResolveVariableSlotNilValue = 0x2B328u;
// How many entries the table keeps, and the lowest upvalue index it can name.
constexpr uint32_t kResolveVariableSlotSlotCount = 0xFFFFD8EEu;
constexpr uint32_t kResolveVariableSlotIndexFloor = 0xFFFFD8F1u;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kResolveVariableSlotSaveFrameBytes = 12u;
constexpr uint32_t kResolveVariableSlotResumeWordBytes = 4u;
}  // namespace

// Turns a variable descriptor into the slot address the emitter writes to. The
// descriptor's header holds a kind that says whether the variable is a plain
// slot, an upvalue, a numeric key or one still to be looked up by name; looking
// a name up is the only step here that can suspend.
uint32_t ResolveVariableSlot(EngineContext* ctx, uint32_t state, uint32_t variable) {
  EnterFrame();
  // The scratch value the current step computes.
  uint32_t top = 0u;
  // The frame's variable table.
  uint32_t base = 0u;
  // The descriptor table's own index, and the address kept for a miss.
  uint32_t offset = 0u;
  uint32_t fallback = 0u;
  // The kind of variable the descriptor names, and the slot that comes of it.
  uint32_t kind = 0u;
  uint32_t slot = 0u;
  // The base of the save frame and the stack pointer it comes off.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  // The call a rewinding pass re-issues, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back in the order they
    // were written.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kResolveVariableSlotSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    variable = LoadU32(ctx, frame + 4u);
    slot = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word. It names the call that was in flight, so
    // only that call is issued again.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kResolveVariableSlotResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A positive descriptor is an index into the frame's local table, whose
    // entries are sixteen bytes; an entry past the table's own bound resolves to
    // the nil address instead.
    if ((int32_t)variable >= 1) {
      top = LoadU32(ctx, state + 12u) + (variable << (4u & 31)) - 16u;
      variable = kResolveVariableSlotNilValue;
      if (LoadU32(ctx, state + 8u) > top) {
        variable = top;
      }
    } else if ((int32_t)variable >= (int32_t)kResolveVariableSlotIndexFloor) {
      // A negative index names an upvalue, which sits in the frame's upvalue
      // table.
      variable = LoadU32(ctx, state + 8u) + (variable << (4u & 31));
    } else {
      // Otherwise the descriptor is an index into the variable table, told apart
      // by the offset it carries.
      offset = variable + 10002u;
      if (offset == 2u) {
        // A slot stored in the frame's own record.
        variable = LoadU32(ctx, state + 16u) + 96u;
      } else if (offset == 1u) {
        // The variable already owns a slot: hand back the address pair it holds.
        top = LoadU32(ctx, state + 20u);
        top = LoadU32(ctx, top + 4u);
        top = LoadU32(ctx, top);
        variable = LoadU32(ctx, top + 12u);
        top = 5u;
        StoreU32(ctx, state + 96u, top);
        StoreU32(ctx, state + 88u, variable);
        variable = state + 88u;
      } else if (offset == 0u) {
        // The frame's argument area.
        variable = state + 72u;
      } else {
        // The table keeps its entry count in its own header; an index past that
        // count resolves to the nil address instead.
        fallback = kResolveVariableSlotNilValue;
        base = LoadU32(ctx, state + 20u);
        base = LoadU32(ctx, base + 4u);
        base = LoadU32(ctx, base);
        top = base + ((0u - variable) << (4u & 31)) - kResolveVariableSlotEntryBase;
        if ((int32_t)LoadU8(ctx, base + 7u) < (int32_t)(kResolveVariableSlotSlotCount - variable)) {
          variable = fallback;
        } else {
          variable = top;
        }
      }
    }
    // The descriptor's header names the kind three steps below the base.
    kind = LoadU32(ctx, variable + 8u) - 3u;
    slot = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The kind decides where the slot text comes from. Kind zero is looked up by
    // name below; three and anything else fall straight through to the answer.
    switch (kind) {
      case 0: goto lookup_by_name;
      case 1: goto use_upvalue;
      case 2: goto use_numeric_key;
      case 3: goto have_slot;
      case 4: goto use_local;
      default: goto have_slot;
    }
  use_upvalue:;
    slot = LoadU32(ctx, variable);
    slot = LoadU32(ctx, slot + 12u);
    LeaveFrame();
    return slot;
  use_local:;
    slot = LoadU32(ctx, variable);
    slot = LoadU32(ctx, slot + 16u);
    LeaveFrame();
    return slot;
  use_numeric_key:;
    slot = LoadU32(ctx, variable);
    slot = FindNumericKeySlot(ctx, slot);
    LeaveFrame();
    return slot;
  lookup_by_name:;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // Look the name up in the frame's string tables; this is the step that can
    // suspend. Until it returns, the state keeps its old value.
    top = NumberToString(ctx, state, variable);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
    state = top;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A lookup that found nothing leaves the state clear, and there is no slot.
    if (state != 0u) {
      top = LoadU32(ctx, variable);
      slot = LoadU32(ctx, top + 12u);
    }
  }
have_slot:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return slot;
  }
  TrapUnreachable();
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; the
  // words go out in the order a rewind reads them back.
  frame = ReturnFrameSp(ctx);
  top = LoadU32(ctx, frame);
  StoreU32(ctx, top, resume_code);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kResolveVariableSlotResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, variable);
  StoreU32(ctx, frame + 8u, slot);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + 12u;
  StoreU32(ctx, sp, top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
