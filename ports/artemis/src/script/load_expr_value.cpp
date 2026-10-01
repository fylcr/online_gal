#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void GetIndexedValue(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

constexpr uint32_t kLoadExprValueTagFloor = 0xFFFFD8EEu;   // lowest synthetic tag
constexpr uint32_t kLoadExprValueIndexBase = 0xFFFFD8F1u;  // first regular tag
constexpr uint32_t kLoadExprValueSpecialBase = 10002u;     // maps synthetic tags onto 0..2
constexpr uint32_t kLoadExprValueTableBias = 0x27118u;     // bias of the entry table
constexpr uint32_t kLoadExprValueSentinel = 0x2B328u;      // "no value" result
constexpr uint32_t kLoadExprValueSaveFrameBytes = 12u;     // words saved below the resume code
constexpr uint32_t kLoadExprValueResumeWordBytes = 4u;     // resume code slot

}  // namespace

// Reads the value the expression tag names, through the indexed getter so the
// walk can suspend.  The getter can suspend, so the state, the tag and the
// container are kept in the frame.
void LoadExprValue(EngineContext* ctx, uint32_t state, uint32_t tag) {
  EnterFrame();
  uint32_t owner = 0u;   // the container the value is read from
  uint32_t table = 0u;   // the entry table a dynamic tag indexes
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kLoadExprValueSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    tag = LoadU32(ctx, frame + 4u);
    owner = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kLoadExprValueResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    owner = state;
    if ((int32_t)tag >= 1) {
      // A regular slot: the tag indexes the frame's value table.
      tag = LoadU32(ctx, state + 12u) + (tag << 4u) - 16u;
      tag = LoadU32(ctx, state + 8u) > tag ? tag : kLoadExprValueSentinel;
      goto value_ready;
    }
    if ((int32_t)tag >= (int32_t)kLoadExprValueIndexBase) {
      // A frame slot just above the synthetic tags.
      tag = LoadU32(ctx, state + 8u) + (tag << 4u);
      goto value_ready;
    }
    switch (tag + kLoadExprValueSpecialBase) {
      case 0: goto slot_72;
      case 1: goto slot_88;
      case 2: goto slot_96;
      default: goto entry_table;
    }
slot_96:
    tag = LoadU32(ctx, state + 16u) + 96u;
    goto value_ready;
slot_88:
    tag = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u)) + 12u);
    StoreU32(ctx, state + 96u, 5u);
    StoreU32(ctx, state + 88u, tag);
    tag = state + 88u;
    goto value_ready;
slot_72:
    tag = state + 72u;
    goto value_ready;
entry_table:
    // A dynamic name: the tag indexes the container's entry table.
    table = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
    tag = (int32_t)LoadU8(ctx, table + 7u) < (int32_t)(kLoadExprValueTagFloor - tag)
        ? kLoadExprValueSentinel
        : table + ((0u - tag) << 4u) - kLoadExprValueTableBias;
value_ready:
    state = LoadU32(ctx, state + 8u) - 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // GetIndexedValue can suspend, so the values that outlive it are in the frame.
    GetIndexedValue(ctx, owner, tag, state, state);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kLoadExprValueResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, tag);
    StoreU32(ctx, frame + 8u, owner);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kLoadExprValueSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
