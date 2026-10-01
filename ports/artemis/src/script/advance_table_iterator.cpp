#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t NextTableEntry(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key);

namespace {

constexpr uint32_t kAdvanceTableHeadBias = 0x27118u;  // distance the head sits above the list
constexpr uint32_t kAdvanceTableSentinel = 0x2B328u;  // entry for a cursor with no element
constexpr uint32_t kAdvanceTableEndTag = 0xFFFFD8EEu;  // cursor tag: one past the last (-10002)
constexpr uint32_t kAdvanceTableIndexBase = 0xFFFFD8F1u;  // first cursor that names a real element
constexpr uint32_t kAdvanceTableStepBack = 0xFFFFFFF0u;  // the -one-element step: -16 bytes
constexpr uint32_t kAdvanceTableTagBias = 10002u;  // added to a cursor to reach the tag switch
constexpr uint32_t kAdvanceTableSaveFrameBytes = 16u;  // saved state words below the resume code
constexpr uint32_t kAdvanceTableResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Steps the table cursor to the next present element and answers whether one
// exists.  The step is a suspendable call, so the cursor, the entry the call
// needs and the state it is asked about are kept in the frame.
uint32_t AdvanceTableIterator(EngineContext* ctx, uint32_t state, uint32_t table) {
  EnterFrame();
  uint32_t saved_state = 0u;
  uint32_t table_end = 0u;
  uint32_t head = 0u;
  uint32_t entry = 0u;
  uint32_t resume_point = 0u;
  uint32_t called = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kAdvanceTableSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    table_end = LoadU32(ctx, frame + 8u);
    saved_state = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kAdvanceTableResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    saved_state = state;
    if ((int32_t)table >= 1) {
      // The cursor sits one past an element, so that element is what this step
      // hands back; an element outside the table gives the sentinel instead.
      // Entries are 16 bytes apart.
      table = LoadU32(ctx, state + 12u) + (table << 4u) - 16u;
      entry = LoadU32(ctx, state + 8u) > table ? table : kAdvanceTableSentinel;
      goto entry_ready;
    }
    if ((int32_t)table >= (int32_t)kAdvanceTableIndexBase) {
      // A real index: address the element it names.
      entry = LoadU32(ctx, state + 8u) + (table << 4u);
      goto entry_ready;
    }
    switch (table + kAdvanceTableTagBias) {
      case 0u: goto cursor_start;
      case 1u: goto cursor_reset;
      case 2u: goto cursor_end;
      default: goto cursor_back;
    }
  cursor_end:
    entry = LoadU32(ctx, state + 16u) + 96u;
    goto entry_ready;
  cursor_reset:
    // Restart: the element list and the mode that says the iterator is walking
    // it are remembered on the state, and the cursor becomes that pair.
    table = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u)) + 12u);
    StoreU32(ctx, state + 96u, 5u);
    StoreU32(ctx, state + 88u, table);
    entry = state + 88u;
    goto entry_ready;
  cursor_start:
    entry = state + 72u;
    goto entry_ready;
  cursor_back:
    // A cursor below every tag walks the element list backwards; the sentinel
    // stands in when the walk runs off the head of the list.
    head = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
    entry = (int32_t)LoadU8(ctx, head + 7u) < (int32_t)(kAdvanceTableEndTag - table)
        ? kAdvanceTableSentinel
        : head + ((0u - table) << 4u) - kAdvanceTableHeadBias;
  entry_ready:
    table = LoadU32(ctx, entry);
    table_end = LoadU32(ctx, state + 8u) - 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    called = NextTableEntry(ctx, saved_state, table, table_end);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    table = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The step succeeded (16 bytes) or failed (16 bytes back), and either way
    // the cursor itself is the answer.
    StoreU32(ctx, state + 8u,
        LoadU32(ctx, state + 8u) + (table != 0u ? 16u : kAdvanceTableStepBack));
    result = table;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kAdvanceTableResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, table);
    StoreU32(ctx, frame + 8u, table_end);
    StoreU32(ctx, frame + 12u, saved_state);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kAdvanceTableSaveFrameBytes);
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
