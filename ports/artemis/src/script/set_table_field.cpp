// set_table_field.cpp — store the top value record into a container field.
//
// SetTableField is the writing twin of PushTableField. The caller hands a pseudo
// index naming a script value; the shared ladder in value_slot.h resolves it to
// the 16-byte value record the field lives in, and that record's own first word
// names the container the field belongs to. The numeric get-or-create lookup
// then answers with the record the key names, creating it when the container
// still has no entry for that key, and the value sitting on top of the value
// stack is copied into that record — a whole 16 bytes, eight at a time.
//
// The copy is followed by the incremental collector's write barrier. Three gates
// have to hold before the container is touched: the stored value's type tag must
// read collectable, the object just stored must not be white, and the container
// must still be black. Only then is the container's mark cleared and the record
// relinked onto the gray list ahead of the list head, so the object just stored
// cannot be swept before the container is scanned again. A gate that fails jumps
// past the relink to the common tail, and every path consumes the pushed value:
// the value stack retreats one whole record.
//
// The body keeps the asyncify shape this ABI demands. Three phases meet in one
// entry — the running body, the replay that pops the return frame this entry
// pushed when the lookup suspended, and the save point that hands the frame and
// the call's result word back to the driver. The return-frame pointer is not
// held in a register here: the context names a guest word, and that word holds
// the pointer, so every pop and push is a read-modify-write of it. The lookup is
// the body's only guest call, so it is the only point where the phase can
// change. The resumed result word gates that call: it reads zero both while
// running and after a replay, so this entry's single call site always runs, and
// only a word the driver staged by some other route would skip it. The frame
// holds the five values a replay needs — the state pointer, the value-stack top
// captured before the call （the lookup may push records of its own, and the copy
// addresses the record that was on top when this entry began, not whatever the
// call left behind）, the key, the resolved value slot and the container.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "script/value_slot.h"
#include "util/engine_context.h"

namespace artc {

// Forward declaration — provisional: this is the generated f898, the numeric
// get-or-create twin of LookupValue（where LookupValue misses to the shared nil
// record, this one grows and rehashes the container's hash part and answers with
// the record that now holds the key）, and is not yet registered in
// rewrite-names.tsv.
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key);

void SetTableField(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t key) {
  EnterFrame();
  // The call's result word: the driver's staged answer on a replay, the lookup's
  // answer once it returns, and the zero this entry publishes when it suspends.
  uint32_t resume_result = 0u;
  // The container that owns the field. On the rewind pass this word carries the
  // frame base first, exactly as the generated body does, and the container is
  // read out of the frame into it afterwards.
  uint32_t table = 0u;
  // The 16-byte value record the field lives in.
  uint32_t slot = 0u;
  // The object the write barrier rewrites: the container the resolved slot
  // points at. The generated body reuses the key word for this, but the key is
  // still live in the suspend path below, so it is kept apart.
  uint32_t target = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewind: lower the pointer word by the frame size, which exposes this
    // entry's frame, then read the five words back in the order the save point
    // wrote them.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 20u);
    table = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, table);
    index = LoadU32(ctx, table + 4u);
    key = LoadU32(ctx, table + 8u);
    slot = LoadU32(ctx, table + 12u);
    table = LoadU32(ctx, table + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewind: pop the result word the save point left one word below that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_result = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: resolve the index through the shared ladder, take the container
    // the slot's first word names, and capture the value-stack top. The ladder
    // consumes the index here; from this point the word holds the captured top,
    // and every read of it below is that top, never the index again.
    slot = ResolveValueSlot(ctx, state, index);
    table = LoadU32(ctx, slot);
    index = LoadU32(ctx, state + 8u);
  }
  // The lookup runs unconditionally while running, and on a replay only for the
  // call site that suspended, which is this one: a zero result word means the
  // call has to be issued again rather than trusted.
  bool const at_own_site = (resume_result == 0u);
  bool const past_running = (ResumePhase(ctx) != kResumeRunning);
  if (past_running ? at_own_site : true) {
    resume_result = GetTableFieldByNumber(ctx, state, table, key);
    // Staged for the save point below: suspending hands the driver a zero.
    uint32_t const abandoned = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: publish that zero into the result slot the driver left, then
      // append this entry's frame behind it and answer the driver. The top
      // pushed is the captured one, and the key is still a key, because the
      // call never handed an answer back.
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), abandoned);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      uint32_t const frame_base = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, frame_base, state);
      StoreU32(ctx, frame_base + 4u, index);
      StoreU32(ctx, frame_base + 8u, key);
      StoreU32(ctx, frame_base + 12u, slot);
      StoreU32(ctx, frame_base + 16u, table);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 20u);
      LeaveFrame();
      return;
    }
    // The lookup completed: its answer is the record the caller writes to, so
    // the key word stops being a key and becomes that record.
    key = resume_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: copy the top record — the one captured above, not whatever the
    // lookup may have pushed since — into the record the lookup returned. The
    // source record sits one whole record below the captured top, so the word
    // is lowered to that source and read twice before it is reused.
    index = index - 16u;
    StoreU64(ctx, key, LoadU64(ctx, index));
    StoreU32(ctx, key + 8u, LoadU32(ctx, index + 8u));
    // Then the write barrier, three gates deep. The live top is re-read first,
    // because the lookup may have moved the value stack under this entry; the
    // captured top above was the copy source and takes no part in the gates.
    // Each gate that fails jumps straight to the common tail below.
    index = LoadU32(ctx, state + 8u);
    if (static_cast<int32_t>(LoadU32(ctx, index - 8u)) < 4) {
      goto consume_value;
    }
    // Gate one: the stored value's own type tag has to read collectable.
    if ((LoadU8(ctx, LoadU32(ctx, index - 16u) + 5u) & 3u) == 0u) {
      goto consume_value;
    }
    // Gate two: the object just stored must not be white.
    target = LoadU32(ctx, slot);
    if ((LoadU8(ctx, target + 5u) & 4u) == 0u) {
      goto consume_value;
    }
    // Gate three: the container is black, so its mark is cleared and it is
    // relinked onto the gray list ahead of the current head.
    index = LoadU32(ctx, state + 16u);
    StoreU8(ctx, target + 5u, LoadU8(ctx, target + 5u) & 251u);
    StoreU32(ctx, target + 24u, LoadU32(ctx, index + 40u));
    StoreU32(ctx, index + 40u, target);
    // The relink moved the word the top is read from, so it is read once more;
    // this is the only gate that leaves a fresher value behind.
    index = LoadU32(ctx, state + 8u);
  consume_value:
    // Every gate meets here: the pushed value is consumed, and the top stored
    // is the one the last gate to run published.
    StoreU32(ctx, state + 8u, index - 16u);
  }
  LeaveFrame();
}

}  // namespace artc
