#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key);
void FormatSourceId(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array);
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2,
                             uint32_t a3);

namespace {

constexpr uint32_t kTracebackEmptyString = 0x2A7C6u;  // the empty name string
constexpr uint32_t kTracebackTail = 0x2A7C7u;  // "tail"
constexpr uint32_t kTracebackTailCall = 0x2A7CCu;  // "=(tail call)"
constexpr uint32_t kTracebackC = 0x2A7D9u;  // "=[C]"
constexpr uint32_t kTracebackUnknownSource = 0x2A7DEu;  // "?"
constexpr uint32_t kTracebackMain = 0x2A7E0u;  // "main"
constexpr uint32_t kTracebackUnknownFunc = 0x2A7E5u;  // "?"
constexpr uint32_t kTracebackMinusOne = 0xFFFFFFFFu;  // -1 as a raw word
constexpr uint32_t kTracebackTagBias = 76u;  // 'L'; tag letters are biased by it
constexpr uint32_t kTracebackSaveFrameBytes = 36u;  // live state bytes pushed on suspend
constexpr uint32_t kTracebackResumeWordBytes = 4u;  // the resume word above the frame

}  // namespace

// Formats one frame of a traceback: looks the function up in the debug pool,
// walks the sections that ride along with its name, and fills the frame's
// source, line and name fields.
//
// Live words across a suspend: the engine state, the name buffer, the output
// frame, the section cursor, the current tag, the descriptor, the scan cursor,
// the found flag and the pending stack-grow flag, plus the frame base.
uint32_t FormatTracebackFrame(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t out) {
  EnterFrame();
  uint32_t line = 0u;
  uint32_t resume_point = 0u;
  uint32_t need_grow = 0u;
  uint32_t line_slot = 0u;
  uint32_t upvalue_table = 0u;
  uint32_t source_out = 0u;
  uint32_t local_slot = 0u;
  uint32_t kind_out = 0u;
  uint32_t cursor = 0u;
  uint32_t entry = 0u;
  uint32_t tag = 0u;
  uint32_t frame = 0u;
  uint32_t offset = 0u;
  uint32_t descriptor = 0u;
  uint32_t scan = 0u;
  uint32_t found = 0u;
  uint32_t result = 0u;
  uint32_t value = 0u;
  // Rewind: drop the 36-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kTracebackSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    out = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    tag = LoadU32(ctx, frame + 16u);
    descriptor = LoadU32(ctx, frame + 20u);
    scan = LoadU32(ctx, frame + 24u);
    found = LoadU32(ctx, frame + 28u);
    need_grow = LoadU32(ctx, frame + 32u);
    name = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kTracebackResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A leading '>' marks a synthetic frame whose descriptor is on the stack.
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = (LoadU8(ctx, name) == 62u);
  }
  result = line;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (cursor != 0u) {
      cursor = LoadU32(ctx, state + 8u) - 16u;
      descriptor = LoadU32(ctx, cursor);
      StoreU32(ctx, state + 8u, cursor);
      name = name + 1u;
      goto lookup_entry;
    }
    cursor = LoadU32(ctx, out + 96u);
    if (cursor == 0u) goto tail_call_frame;
    entry = LoadU32(ctx, state + 40u) + (cursor * 24u);
    descriptor = LoadU32(ctx, LoadU32(ctx, entry + 4u));
    // The frame points either at the stack entry or at the debug entry.
    lookup_entry:;
    if (descriptor == 0u) goto tail_call_frame;
    tag = LoadU8(ctx, name);
    cursor = (tag == 0u);
    if (cursor != 0u) {
      found = 1u;
      goto name_scan_done;
    }
    source_out = out + 36u;
    line_slot = entry - 12u;
    local_slot = entry - 20u;
    upvalue_table = entry - 24u;
    kind_out = out + 4u;
    found = 1u;
    cursor = name;
    scan_sections:;
    scan = cursor;
    // The byte after the name says which debug section it starts: 'L' and
    // 'f' carry nothing further, 'S' the source, 'l' the locals, 'n' the name
    // and 'u' the upvalues.
    tag = static_cast<uint32_t>(static_cast<int32_t>(tag << (24u & 31)) >> (24u & 31));
    switch (tag - kTracebackTagBias) {
      case 0:
      case 26: goto tag_handled;
      case 7: goto source_section;
      case 32: goto local_section;
      case 34: goto name_section;
      case 41: goto upvalue_section;
      default: goto unknown_tag;
    }
    // 'S': the source name and its chunk id.
    source_section:;
    if (LoadU8(ctx, descriptor + 6u) != 0u) {
      StoreU32(ctx, out + 28u, kTracebackMinusOne);
      cursor = kTracebackC;
      StoreU32(ctx, out + 16u, kTracebackC);
      offset = kTracebackUnknownSource;
      value = kTracebackMinusOne;
      goto line_stored;
    }
    cursor = LoadU32(ctx, LoadU32(ctx, descriptor + 16u) + 32u) + 16u;
    StoreU32(ctx, out + 16u, cursor);
    tag = LoadU32(ctx, LoadU32(ctx, descriptor + 16u) + 60u);
    StoreU32(ctx, out + 28u, tag);
    offset = tag ? kTracebackUnknownFunc : kTracebackMain;
    value = LoadU32(ctx, LoadU32(ctx, descriptor + 16u) + 64u);
    line_stored:;
    tag = value;
    StoreU32(ctx, out + 12u, offset);
    StoreU32(ctx, out + 32u, tag);
    FormatSourceId(ctx, source_out, cursor, 60u);
    goto tag_handled;
    local_section:;
    tag = kTracebackMinusOne;
    if (entry == 0u) goto line_info_done;
    cursor = LoadU32(ctx, entry + 4u);
    if (LoadU32(ctx, cursor + 8u) != 6u) goto line_info_done;
    offset = LoadU32(ctx, cursor);
    if (LoadU8(ctx, offset + 6u) != 0u) goto line_info_done;
    if (entry != LoadU32(ctx, state + 20u)) {
      line = LoadU32(ctx, entry + 12u);
      goto scan_line_info;
    }
    line = LoadU32(ctx, state + 24u);
    StoreU32(ctx, entry + 12u, line);
    offset = LoadU32(ctx, cursor);
    scan_line_info:;
    cursor = LoadU32(ctx, offset + 16u);
    offset = line - LoadU32(ctx, cursor + 12u);
    if (static_cast<int32_t>(offset) < 4) goto line_info_done;
    cursor = LoadU32(ctx, cursor + 20u);
    if (cursor == 0u) {
      StoreU32(ctx, out + 20u, 0u);
      goto tag_handled;
    }
    tag = LoadU32(ctx,
                  ((static_cast<uint32_t>(static_cast<int32_t>(offset) >> (2u & 31)) - 1u)
                   << (2u & 31)) + cursor);
    line_info_done:;
    StoreU32(ctx, out + 20u, tag);
    goto tag_handled;
    upvalue_section:;
    StoreU32(ctx, out + 24u, LoadU8(ctx, descriptor + 7u));
    goto tag_handled;
    // 'n': the variable the frame is running in, if the debug info names it.
    name_section:;
    if (entry == 0u) goto no_name_info;
    cursor = LoadU32(ctx, entry + 4u);
    if (LoadU32(ctx, cursor + 8u) != 6u) goto check_local_entry;
    if (LoadU8(ctx, LoadU32(ctx, cursor) + 6u) != 0u) goto check_local_entry;
    if (static_cast<int32_t>(LoadU32(ctx, entry + 20u)) > 0) goto no_name_info;
    check_local_entry:;
    tag = LoadU32(ctx, local_slot);
    if (LoadU32(ctx, tag + 8u) != 6u) goto no_name_info;
    cursor = LoadU32(ctx, tag);
    if (LoadU8(ctx, cursor + 6u) != 0u) goto no_name_info;
    offset = LoadU32(ctx, LoadU32(ctx, cursor + 16u) + 12u);
    if (upvalue_table != LoadU32(ctx, state + 20u)) {
      line = LoadU32(ctx, line_slot);
      goto local_line_ready;
    }
    line = LoadU32(ctx, state + 24u);
    StoreU32(ctx, line_slot, line);
    cursor = LoadU32(ctx, tag);
    local_line_ready:;
    cursor = LoadU32(ctx, (offset + (line - LoadU32(ctx, LoadU32(ctx, cursor + 16u) + 12u))) - 4u);
    tag = (cursor & 63u) - 28u;
    if (tag > 5u) goto no_name_info;
    if (((1u << (tag & 31)) & 35u) == 0u) goto no_name_info;
    cursor = ResolveVariableKind(ctx, state, upvalue_table, ((cursor >> (6u & 31)) & 255u),
                                 kind_out);
    StoreU32(ctx, out + 8u, cursor);
    if (cursor != 0u) goto tag_handled;
    no_name_info:;
    StoreU32(ctx, out + 4u, 0u);
    StoreU32(ctx, out + 8u, kTracebackEmptyString);
    goto tag_handled;
    unknown_tag:;
    found = 0u;
    tag_handled:;
    cursor = scan + 1u;
    tag = LoadU8(ctx, scan + 1u);
    if (tag != 0u) goto scan_sections;
    name_scan_done:;
    out = (FindPoolEntryByTagByte(ctx, name, 102u) == 0u);
    if (out != 0u) goto check_c_call;
    out = LoadU32(ctx, state + 8u);
    StoreU32(ctx, out + 8u, 6u);
    StoreU32(ctx, out, descriptor);
    out = LoadU32(ctx, state + 8u);
    value = 0u;
    goto value_ready;
    // No debug entry: the frame is a tail call, or a function with no name.
  tail_call_frame:;
    StoreU64(ctx, out + 28u, 18446744073709551615ull);
    StoreU32(ctx, out + 20u, kTracebackMinusOne);
    StoreU32(ctx, out + 12u, kTracebackTail);
    StoreU32(ctx, out + 4u, kTracebackEmptyString);
    StoreU32(ctx, out + 8u, kTracebackEmptyString);
    StoreU32(ctx, out + 16u, kTracebackTailCall);
    cursor = out + 36u;
    FormatSourceId(ctx, cursor, kTracebackTailCall, 60u);
    descriptor = 0u;
    StoreU32(ctx, out + 24u, 0u);
    out = (FindPoolEntryByTagByte(ctx, name, 102u) == 0u);
    if (out != 0u) {
      found = 1u;
      tag = 1u;
      goto check_function_kind;
    }
    out = LoadU32(ctx, state + 8u);
    StoreU32(ctx, out + 8u, 0u);
    found = 1u;
    value = 1u;
    value_ready:;
    tag = value;
    cursor = state;
    scan = (static_cast<int32_t>((LoadU32(ctx, cursor + 28u) - out)) <= 16);
  }
  result = ResumePhase(ctx) ? result : scan;
  if (result != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      result = LoadU32(ctx, state + 8u);
    } else {
      // Rewinding: the restored frame offset is reused as it stands.
      result = out;
    }
    out = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    out = out + 16u;
    StoreU32(ctx, cursor + 8u, out);
  }
  check_function_kind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    name = (FindPoolEntryByTagByte(ctx, name, 76u) == 0u);
    if (name != 0u) goto finished;
    if (tag != 0u) goto function_frame;
    goto check_upvalues;
  }
  check_c_call:;
  if (ResumePhase(ctx) == kResumeRunning) {
    name = (FindPoolEntryByTagByte(ctx, name, 76u) == 0u);
    if (name != 0u) goto finished;
  }
  check_upvalues:;
  if (ResumePhase(ctx) == kResumeRunning) {
    name = (LoadU8(ctx, descriptor + 6u) == 0u);
    if (name != 0u) goto copy_upvalues;
  }
  function_frame:;
  if (ResumePhase(ctx) == kResumeRunning) {
    out = LoadU32(ctx, state + 8u);
    StoreU32(ctx, out + 8u, 0u);
    goto upvalues_done;
  }
  // The function has upvalues: copy their names into a fresh table.
  copy_upvalues:;
  out = ResumePhase(ctx) ? out : 0u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    entry = AllocateTable(ctx, state, 0u, 0u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    name = entry;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = LoadU32(ctx, descriptor + 16u);
    tag = (static_cast<int32_t>(LoadU32(ctx, cursor + 48u)) >= 1);
  }
  if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = LoadU32(ctx, cursor + 20u);
    }
    copy_upvalue_entries:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tag = LoadU32(ctx, (out << (2u & 31)) + cursor);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      entry = GetTableFieldByNumber(ctx, state, name, tag);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      scan = entry;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scan + 8u, 1u);
      StoreU32(ctx, scan, 1u);
      out = out + 1u;
      tag = (static_cast<int32_t>(out) <
             static_cast<int32_t>(LoadU32(ctx, LoadU32(ctx, descriptor + 16u) + 48u)));
      if (tag != 0u) goto copy_upvalue_entries;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    out = LoadU32(ctx, state + 8u);
    StoreU32(ctx, out + 8u, 5u);
    StoreU32(ctx, out, name);
    out = LoadU32(ctx, state + 8u);
  }
  upvalues_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = (static_cast<int32_t>((LoadU32(ctx, state + 28u) - out)) <= 16);
    name = state;
  }
  // Grow the value stack if the frame no longer fits in it.
  need_grow = ResumePhase(ctx) ? need_grow : cursor;
  if (need_grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      result = LoadU32(ctx, state + 8u);
    } else {
      // Rewinding: the restored frame offset is reused as it stands.
      result = out;
    }
    out = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, name + 8u, out + 16u);
  }
  finished:;
  if (ResumePhase(ctx) == kResumeRunning) {
    result = found;
    goto exit_path;
  }
  TrapUnreachable();
  suspend_point:;
  // Suspend: push the resume point, then the 36-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kTracebackResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, name);
  StoreU32(ctx, frame + 8u, out);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, tag);
  StoreU32(ctx, frame + 20u, descriptor);
  StoreU32(ctx, frame + 24u, scan);
  StoreU32(ctx, frame + 28u, found);
  StoreU32(ctx, frame + 32u, need_grow);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kTracebackSaveFrameBytes);
  result = 0u;
  exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
