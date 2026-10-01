#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t ResolveTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t GrowArray(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);

namespace {

constexpr uint32_t kAddConstantOverflowMessage = 0x2A3AFu;  // "constant table overflow"
constexpr uint32_t kAddConstantCellLimit = 0x3FFFFu;  // most cells the table may hold
constexpr uint32_t kAddConstantIntOverflow = 0x80000000u;  // int a double out of range converts to
constexpr uint32_t kAddConstantDoubleKind = 3u;  // value-kind tag of a double cell
constexpr uint32_t kAddConstantSaveFrameBytes = 32u;  // saved state words below the resume code
constexpr uint32_t kAddConstantResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Interns one key/value pair into the function constant table: the cell that
// the key resolves to is looked at first, the pair is appended when the table
// grew, and a table that outgrows its limit traps.  Two calls can suspend, so
// every value that outlives them is kept in the frame.
uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value) {
  EnterFrame();
  uint32_t slot = 0u;
  uint32_t called = 0u;
  uint32_t pool = 0u;
  uint32_t number = 0u;
  uint32_t count_addr = 0u;
  uint32_t pool_base = 0u;
  uint32_t table = 0u;
  uint32_t resume_point = 0u;
  uint32_t flag = 0u;
  uint32_t result = 0u;
  uint32_t target = 0u;
  uint32_t source = 0u;
  uint32_t index = 0u;
  double raw = 0.0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 32-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kAddConstantSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    key = LoadU32(ctx, frame + 4u);
    value = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
    pool = LoadU32(ctx, frame + 16u);
    count_addr = LoadU32(ctx, frame + 20u);
    table = LoadU32(ctx, frame + 24u);
    flag = LoadU32(ctx, frame + 28u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kAddConstantResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    table = LoadU32(ctx, func_state + 16u);
    slot = LoadU32(ctx, func_state + 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    called = ResolveTableField(ctx, table, slot, key);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    slot = called;
  }
  // The running pass looks at the cell the key resolved to: a cell that is
  // already a double converts straight back to an int, anything else gets the
  // pair interned behind it.  The rewinding pass keeps the flag it saved.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU32(ctx, slot + 8u) == kAddConstantDoubleKind) {
      raw = LoadF64(ctx, slot);
      if (wasm_fabs(raw) < 2147483648.0) {
        result = I32_TRUNC_S_F64(raw);
        goto exit_path;
      }
      result = kAddConstantIntOverflow;
      goto exit_path;
    }
    pool = LoadU32(ctx, func_state);
    key = LoadU32(ctx, pool + 40u);
    number = LoadU32(ctx, func_state + 40u);
    StoreU32(ctx, slot + 8u, kAddConstantDoubleKind);
    StoreF64(ctx, slot, (double)(int32_t)number);
    count_addr = pool + 40u;
    slot = LoadU32(ctx, pool + 40u);
    flag = (uint32_t)((int32_t)slot <= (int32_t)LoadU32(ctx, func_state + 40u));
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, pool + 8u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      // The pool may move, so the slot index is read back from it afterwards.
      called = GrowArray(ctx, table, slot, count_addr, 16u, kAddConstantCellLimit,
          kAddConstantOverflowMessage);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      slot = called;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, pool + 8u, slot);
      slot = LoadU32(ctx, pool + 40u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Clear the kind word of every cell between the resolved one and the free
    // slot, then copy the pair into the free slot.  A cell holds 8 bytes of
    // payload with its kind word at +8, so the walk strides by 16.
    pool_base = LoadU32(ctx, pool + 8u);
    if ((int32_t)key < (int32_t)slot) {
    clear_cells:
      StoreU32(ctx, pool_base + (key << 4u) + 8u, 0u);
      key += 1u;
      if ((int32_t)slot > (int32_t)key) goto clear_cells;
    }
    target = pool_base + (LoadU32(ctx, func_state + 40u) << 4u);
    StoreU64(ctx, target, LoadU64(ctx, value));
    StoreU32(ctx, target + 8u, LoadU32(ctx, value + 8u));
    if ((int32_t)LoadU32(ctx, value + 8u) < (int32_t)4u) goto finish;
    source = LoadU32(ctx, value);
    if ((LoadU8(ctx, source + 5u) & 3u) == 0u) goto finish;
    if ((LoadU8(ctx, pool + 5u) & 4u) == 0u) goto finish;
    StoreListEntry(ctx, table, pool, source);
  finish:;
    index = LoadU32(ctx, func_state + 40u);
    StoreU32(ctx, func_state + 40u, index + 1u);
    result = index;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kAddConstantResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, func_state);
    StoreU32(ctx, frame + 4u, key);
    StoreU32(ctx, frame + 8u, value);
    StoreU32(ctx, frame + 12u, slot);
    StoreU32(ctx, frame + 16u, pool);
    StoreU32(ctx, frame + 20u, count_addr);
    StoreU32(ctx, frame + 24u, table);
    StoreU32(ctx, frame + 28u, flag);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kAddConstantSaveFrameBytes);
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
