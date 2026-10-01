#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
uint32_t CompactEntryQueue(EngineContext* ctx, uint32_t state, uint32_t queue);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
void ThrowScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array);
uint32_t GetTableFieldByString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseTypeError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kCallFrameBytes = 60u;             // the save frame: fifteen words
constexpr uint32_t kCallResumeWordBytes = 4u;         // the resume word written ahead of it
constexpr uint32_t kCallScratchBytes = 112u;          // the scratch block a call works in
constexpr uint32_t kCallFunctionTag = 6u;             // the value tag that marks a callable
constexpr uint32_t kCallMarginBytes = 320u;           // the stack room a called function is given
constexpr uint32_t kCallMsgCall = 0x2A8ABu;           // "call"
constexpr uint32_t kCallMsgFieldName = 0x2A8B0u;      // the one-character table field name
constexpr uint32_t kCallMsgStackOverflow = 0x2A8B2u;  // "stack overflow"
constexpr uint32_t kCallMaxElementCount = 0xAAAAAAAu;  // the biggest array count the growth accepts
constexpr uint32_t kCallNoIndex = 0xFFFFFFFFu;        // the sentinel meaning "no entry"

}  // namespace

namespace LuaEngine {

// The script call entry: one suspendable step that calls the value in `target`
// -- through its __call metamethod when the value is a table -- and answers the
// number of results the call left behind.
uint32_t Call(EngineContext* ctx, uint32_t state, uint32_t target, uint32_t callee) {
  EnterFrame();
  uint32_t cursor = 0u;
  uint32_t work = 0u;
  uint32_t frame = 0u;
  uint32_t space = 0u;
  uint32_t table = 0u;
  uint32_t resume_point = 0u;
  uint32_t result = 0u;
  uint32_t offset = 0u;
  uint32_t scratch = 0u;
  uint32_t descriptor = 0u;
  uint32_t arg_count = 0u;
  uint32_t length = 0u;
  uint32_t new_table = 0u;
  uint32_t arg_bytes = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kCallFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    callee = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    work = LoadU32(ctx, frame + 16u);
    space = LoadU32(ctx, frame + 20u);
    table = LoadU32(ctx, frame + 24u);
    result = LoadU32(ctx, frame + 28u);
    offset = LoadU32(ctx, frame + 32u);
    scratch = LoadU32(ctx, frame + 36u);
    descriptor = LoadU32(ctx, frame + 40u);
    arg_count = LoadU32(ctx, frame + 44u);
    length = LoadU32(ctx, frame + 48u);
    new_table = LoadU32(ctx, frame + 52u);
    arg_bytes = LoadU32(ctx, frame + 56u);
    target = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kCallResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The scratch block a call needs while it indexes tables.
    scratch = GuestStackSp(ctx) - kCallScratchBytes;
    GuestStackSp(ctx) = scratch;
    work = (LoadU32(ctx, target + 8u) != kCallFunctionTag);
  }
  if (work != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Not a plain function: look for a __call metamethod.
      result = FetchObjectMetamethod(ctx, state, target, 16u);
      table = LoadU32(ctx, state + 32u);
      work = (LoadU32(ctx, result + 8u) != kCallFunctionTag);
    }
    if (work != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
        RaiseTypeError(ctx, state, target, kCallMsgCall);
        resume_point = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Move the arguments down over the metamethod value.
      cursor = LoadU32(ctx, state + 8u);
      if (cursor > target) {
        do {
          const uint32_t from = cursor - 16u;
          StoreU64(ctx, cursor, LoadU64(ctx, from));
          space = LoadU32(ctx, from + 8u);
          StoreU32(ctx, cursor + 8u, space);
          cursor = from;
        } while (cursor > target);
        cursor = LoadU32(ctx, state + 8u);
      }
      work = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
    }
    if (work != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        work = LoadU32(ctx, state + 44u);
        work = ((int32_t)work < (int32_t)1u) ? (work + 1u) : (work << 1u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
        RelocateElementArray(ctx, state, work);
        resume_point = 1u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, state + 8u, LoadU32(ctx, state + 8u) + 16u);
      target = LoadU32(ctx, state + 32u) + (target - table);
      StoreU64(ctx, target, LoadU64(ctx, result));
      work = LoadU32(ctx, result + 8u);
      StoreU32(ctx, target + 8u, work);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, target);
    work = LoadU32(ctx, state + 32u);
    length = LoadU32(ctx, state + 20u);
    space = LoadU32(ctx, state + 24u);
    StoreU32(ctx, length + 12u, space);
    arg_bytes = target - work;
    target = !LoadU8(ctx, cursor + 6u);
  }
  if (target != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      space = LoadU32(ctx, state + 28u) - LoadU32(ctx, state + 8u);
      descriptor = LoadU32(ctx, cursor + 16u);
      work = LoadU8(ctx, descriptor + 75u);
      const uint32_t descriptor_bytes = work << 4u;
      target = (uint32_t)((int32_t)space <= (int32_t)descriptor_bytes);
    }
    if (target != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        const uint32_t element_size = LoadU32(ctx, state + 44u);
        const uint32_t grown = work + element_size;
        target = ((int32_t)element_size < (int32_t)work) ? grown : (element_size << 1u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        RelocateElementArray(ctx, state, target);
        resume_point = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      work = arg_bytes + LoadU32(ctx, state + 32u);
      table = LoadU8(ctx, descriptor + 74u);
      target = !table;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (target != 0u) {
        // The field is already there: open room for it and skip the store.
        cursor = work + 16u;
        target = cursor + (LoadU8(ctx, descriptor + 73u) << 4u);
        space = (target >= LoadU32(ctx, state + 8u));
        if (space) goto arguments_ready;
        StoreU32(ctx, state + 8u, target);
        goto arguments_ready;
      }
      // Otherwise drop the arguments the call no longer needs.
      cursor = LoadU32(ctx, state + 8u);
      target = (uint32_t)((int32_t)(cursor - work) >> 4u);
      work = target - 1u;
      space = target;
      target = LoadU8(ctx, descriptor + 73u);
      if ((int32_t)space <= (int32_t)target) {
        do {
          StoreU32(ctx, cursor + 8u, 0u);
          cursor = cursor + 16u;
          work = work + 1u;
        } while (work != target);
        StoreU32(ctx, state + 8u, cursor);
        work = target;
      }
      // Without the array-part flag the field stays in the map part of the table.
      space = !(table & 4u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A missing field is stored in the map part of the table.
      if (space) goto table_ready;
      table = LoadU32(ctx, state + 16u);
      length = LoadU32(ctx, table + 68u);
      space = (length >= LoadU32(ctx, table + 64u));
    }
    if (space || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
        EnsureTableArrayRoom(ctx, state);
        resume_point = 3u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      space = LoadU32(ctx, state + 28u) - cursor;
      table = LoadU8(ctx, descriptor + 75u);
      cursor = (uint32_t)((int32_t)space <= (int32_t)(table << 4u));
    }
    if (cursor || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        const uint32_t element_size = LoadU32(ctx, state + 44u);
        length = table + element_size;
        cursor = ((int32_t)element_size < (int32_t)table) ? length : (element_size << 1u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
        RelocateElementArray(ctx, state, cursor);
        resume_point = 4u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    arg_count = (ResumePhase(ctx) == kResumeRunning) ? (work - target) : arg_count;
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      // A suspended callee answers a placeholder, so the result waits in a temporary.
      const uint32_t call_result = AllocateTable(ctx, state, arg_count, 1u);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      new_table = call_result;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning)
        ? (uint32_t)((int32_t)arg_count >= (int32_t)1u) : cursor;
    if (cursor || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // Arguments are copied back to front, so the source offset starts negative.
        length = (uint32_t)(0u - arg_count) << 4u;
        cursor = 0u;
      }
    copy_arguments:;
      if (ResumePhase(ctx) == kResumeRunning) {
        offset = cursor << 4u;
        table = LoadU32(ctx, state + 8u);
        cursor = cursor + 1u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
        const uint32_t call_result = GetTableFieldByNumber(ctx, state, new_table, cursor);
        resume_point = 6u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        result = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        table = offset + (table + length);
        StoreU64(ctx, result, LoadU64(ctx, table));
        StoreU32(ctx, result + 8u, LoadU32(ctx, table + 8u));
        space = (cursor != arg_count);
        if (space) goto copy_arguments;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      const uint32_t call_result = InternString(ctx, state, kCallMsgFieldName, 1u);
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      cursor = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      const uint32_t call_result = GetTableFieldByString(ctx, state, new_table, cursor);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      cursor = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, cursor + 8u, 3u);
      StoreF64(ctx, cursor, (double)(int32_t)arg_count);
      cursor = LoadU32(ctx, state + 8u);
    }
  table_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Shift the arguments up to make room for the results.
      if (target) {
        StoreU32(ctx, state + 8u, cursor + 16u);
        table = cursor - (work << 4u);
        StoreU64(ctx, cursor, LoadU64(ctx, table));
        work = cursor + ((0u - work) << 4u);
        StoreU32(ctx, cursor + 8u, LoadU32(ctx, work + 8u));
        StoreU32(ctx, work + 8u, 0u);
        work = 1u;
        if (target != 1u) {
          do {
            offset = LoadU32(ctx, state + 8u);
            StoreU32(ctx, state + 8u, offset + 16u);
            result = table + (work << kCallResumeWordBytes);
            StoreU64(ctx, offset, LoadU64(ctx, result));
            StoreU32(ctx, offset + 8u, LoadU32(ctx, result + 8u));
            StoreU32(ctx, result + 8u, 0u);
            work = work + 1u;
          } while (work != target);
        }
      }
      if (new_table) {
        target = LoadU32(ctx, state + 8u);
        StoreU32(ctx, state + 8u, target + 16u);
        StoreU32(ctx, target + 8u, 5u);
        StoreU32(ctx, target, new_table);
      }
      target = LoadU32(ctx, state + 32u);
      work = arg_bytes + target;
    }
  arguments_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      target = LoadU32(ctx, state + 20u);
      space = (target != LoadU32(ctx, state + 36u));
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (space) goto frame_ready;
      offset = LoadU32(ctx, state + 48u);
      if ((int32_t)offset >= (int32_t)20001u) goto too_many_elements;
      result = LoadU32(ctx, state + 40u);
      table = offset << 1u;
      // The odd marker only rides on the count compared with the growth limit.
      target = ((table | 1u) <= kCallMaxElementCount);
    }
    if (target || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        space = offset * 24u;
        target = offset * 48u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
        const uint32_t call_result = ReallocateBlock(ctx, state, result, space, target);
        resume_point = 9u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        target = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) goto realloc_ready;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
      const uint32_t call_result = RaiseBlockTooBig(ctx, state);
      resume_point = 10u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      target = call_result;
    }
  realloc_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, state + 48u, table);
      StoreU32(ctx, state + 40u, target);
      StoreU32(ctx, state + 36u, ((table * 24u) + target) - 24u);
      const uint32_t stack_top = LoadU32(ctx, state + 20u);
      target = target + (I32_DIV_S(stack_top - result, 24u) * 24u);
      StoreU32(ctx, state + 20u, target);
      space = ((int32_t)offset < (int32_t)10001u);
      if (space) goto frame_ready;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
      RaiseScriptError(ctx, state, kCallMsgStackOverflow, 0u);
      resume_point = 11u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      target = LoadU32(ctx, state + 20u);
    }
  frame_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Build the frame the called value will run in.
      table = target + 24u;
      StoreU32(ctx, state + 20u, table);
      StoreU32(ctx, target + 24u, cursor);
      StoreU32(ctx, target + 28u, work);
      StoreU32(ctx, state + 12u, cursor);
      offset = cursor + (LoadU8(ctx, descriptor + 75u) << 4u);
      StoreU32(ctx, target + 32u, offset);
      work = LoadU32(ctx, descriptor + 12u);
      StoreU32(ctx, state + 24u, work);
      StoreU32(ctx, target + 40u, callee);
      result = 0u;
      StoreU32(ctx, target + 44u, 0u);
      cursor = LoadU32(ctx, state + 8u);
      if (offset > cursor) {
        do {
          StoreU32(ctx, cursor + 8u, 0u);
          cursor = cursor + 16u;
        } while (offset > cursor);
      }
      StoreU32(ctx, state + 8u, offset);
      if ((LoadU8(ctx, state + 56u) & 1u) == 0u) goto result_ready;
      cursor = work + kCallResumeWordBytes;
      StoreU32(ctx, state + 24u, cursor);
      callee = LoadU32(ctx, state + 68u);
      target = !callee;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (target) goto metamethod_ready;
      target = !LoadU8(ctx, state + 57u);
      if (target) goto metamethod_ready;
      target = LoadU32(ctx, state + 32u);
      StoreU32(ctx, scratch + 28u, kCallNoIndex);
      StoreU32(ctx, scratch + 8u, 0u);
      const uint32_t block_base = LoadU32(ctx, state + 40u);
      StoreU32(ctx, scratch + 104u, I32_DIV_S(table - block_base, 24u));
      work = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - offset) <= (int32_t)kCallMarginBytes);
    }
    if (work || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        work = LoadU32(ctx, state + 44u);
        work = ((int32_t)work < (int32_t)20u) ? (work + 20u) : (work << 1u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
        RelocateElementArray(ctx, state, work);
        resume_point = 12u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = LoadU32(ctx, state + 20u);
      StoreU32(ctx, cursor + 8u, LoadU32(ctx, state + 8u) + kCallMarginBytes);
      StoreU8(ctx, state + 57u, 0u);
      work = scratch + 8u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
      CallTableT0(ctx, callee, state, work);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 13u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, state + 57u, 1u);
      work = LoadU32(ctx, state + 20u);
      callee = LoadU32(ctx, state + 32u);
      target = callee + (offset - target);
      StoreU32(ctx, work + 8u, target);
      StoreU32(ctx, state + 8u, target);
      cursor = LoadU32(ctx, state + 24u);
    }
  metamethod_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, state + 24u, cursor - 4u);
      goto result_ready;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    work = LoadU32(ctx, state + 28u);
    target = (uint32_t)((int32_t)(work - LoadU32(ctx, state + 8u)) <= (int32_t)kCallMarginBytes);
  }
  if (target || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      target = LoadU32(ctx, state + 44u);
      target = ((int32_t)target < (int32_t)20u) ? (target + 20u) : (target << 1u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
      RelocateElementArray(ctx, state, target);
      resume_point = 14u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, state + 20u);
    target = (cursor != LoadU32(ctx, state + 36u));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (target) goto ret_frame_ready;
    table = LoadU32(ctx, state + 48u);
    if ((int32_t)table >= (int32_t)20001u) goto too_many_elements;
    work = LoadU32(ctx, state + 40u);
    target = table << 1u;
    cursor = ((target | 1u) <= kCallMaxElementCount);
  }
  if (cursor || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      space = table * 24u;
      cursor = table * 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
      const uint32_t call_result = ReallocateBlock(ctx, state, work, space, cursor);
      resume_point = 15u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      cursor = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) goto ret_realloc_ready;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
    const uint32_t call_result = RaiseBlockTooBig(ctx, state);
    resume_point = 16u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    cursor = call_result;
  }
ret_realloc_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, state + 48u, target);
    StoreU32(ctx, state + 40u, cursor);
    StoreU32(ctx, state + 36u, ((target * 24u) + cursor) - 24u);
    const uint32_t stack_top = LoadU32(ctx, state + 20u);
    cursor = cursor + (I32_DIV_S(stack_top - work, 24u) * 24u);
    StoreU32(ctx, state + 20u, cursor);
    target = ((int32_t)table < (int32_t)10001u);
    if (target) goto ret_frame_ready;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
    RaiseScriptError(ctx, state, kCallMsgStackOverflow, 0u);
    resume_point = 17u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, state + 20u);
  }
ret_frame_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Build the frame for a plain function call.
    work = cursor + 24u;
    StoreU32(ctx, state + 20u, work);
    table = LoadU32(ctx, state + 32u);
    target = arg_bytes + table;
    StoreU32(ctx, cursor + 28u, target);
    target = target + 16u;
    StoreU32(ctx, cursor + 24u, target);
    StoreU32(ctx, state + 12u, target);
    result = LoadU32(ctx, state + 8u);
    StoreU32(ctx, cursor + 40u, callee);
    callee = result + kCallMarginBytes;
    StoreU32(ctx, cursor + 32u, callee);
    target = ((LoadU8(ctx, state + 56u) & 1u) == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (target) goto ret_entry_ready;
    target = LoadU32(ctx, state + 68u);
    if (!target) goto ret_entry_ready;
    if (LoadU8(ctx, state + 57u) == 0u) goto ret_entry_ready;
    StoreU32(ctx, scratch + 28u, kCallNoIndex);
    StoreU32(ctx, scratch + 8u, 0u);
    const uint32_t block_base = LoadU32(ctx, state + 40u);
    StoreU32(ctx, scratch + 104u, I32_DIV_S(work - block_base, 24u));
    work = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - result) <= (int32_t)kCallMarginBytes);
  }
  if (work || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      work = LoadU32(ctx, state + 44u);
      work = ((int32_t)work < (int32_t)20u) ? (work + 20u) : (work << 1u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
      RelocateElementArray(ctx, state, work);
      resume_point = 18u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, state + 20u);
    StoreU32(ctx, cursor + 8u, LoadU32(ctx, state + 8u) + kCallMarginBytes);
    StoreU8(ctx, state + 57u, 0u);
    work = scratch + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
    CallTableT0(ctx, target, state, work);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 19u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, state + 57u, 1u);
    work = LoadU32(ctx, state + 20u);
    target = LoadU32(ctx, state + 32u);
    StoreU32(ctx, work + 8u, target + (callee - table));
    target = result - table + target;
    StoreU32(ctx, state + 8u, target);
  }
ret_entry_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    target = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, work + 4u)) + 16u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
    const uint32_t call_result = CallTableT2(ctx, target, state);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 20u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    target = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if ((int32_t)target < (int32_t)0u) {
      result = 2u;
      goto result_ready;
    }
    target = LoadU32(ctx, state + 8u) - (target << 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
    const uint32_t call_result = CompactEntryQueue(ctx, state, target);
    resume_point = 21u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    state = call_result;
  }
  result = (ResumePhase(ctx) == kResumeRunning) ? 1u : result;
result_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the scratch block back and answer with the result count.
    GuestStackSp(ctx) = scratch + kCallScratchBytes;
    LeaveFrame();
    return result;
  }
too_many_elements:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 22u) {
    ThrowScriptError(ctx, state, 5u);
    resume_point = 22u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 23u) {
    ThrowScriptError(ctx, state, 5u);
    resume_point = 23u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  TrapUnreachable();
suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kCallResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, target);
  StoreU32(ctx, frame + 8u, callee);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, work);
  StoreU32(ctx, frame + 20u, space);
  StoreU32(ctx, frame + 24u, table);
  StoreU32(ctx, frame + 28u, result);
  StoreU32(ctx, frame + 32u, offset);
  StoreU32(ctx, frame + 36u, scratch);
  StoreU32(ctx, frame + 40u, descriptor);
  StoreU32(ctx, frame + 44u, arg_count);
  StoreU32(ctx, frame + 48u, length);
  StoreU32(ctx, frame + 52u, new_table);
  StoreU32(ctx, frame + 56u, arg_bytes);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kCallFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace LuaEngine
}  // namespace artc
