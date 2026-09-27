// get_table_field_by_string.cpp — string-keyed table get with a suspendable
// general fallback.
//
// GetTableFieldByString resolves table[key] for a key the caller already knows
// is an interned string. The fast path is the VM's string-keyed one: the string
// carries its own cached hash, so the main position is that hash masked down to
// a bucket, and the chain hanging off it is walked with pointer identity as the
// whole comparison — never contents, because the VM interns every string it
// keys a table by. The node's key tag has to read "string" before that identity
// test runs at all, so a node holding some other family's key is skipped rather
// than compared. A match is the answer and leaves with the node's own value
// record, which is why that path skips the general product outright: a resolved
// field must not be routed back through the metamethod chain.
//
// A resolve that finds nothing is not yet an answer, because the field may
// still come from an __index metamethod. Two outcomes mean "no entry", and both
// converge on that fallback: a walk that runs off the end of the chain, and a
// walk that lands on the shared miss record（which is no home for a key
// either）. Both rebuild the key as a string value record in a 16-byte frame
// carved off the guest stack（the value word at +0, the type tag at +8, growing
// down）and hand the get to the general, metamethod-capable product.
//
// That general get can run guest code, so this body is a suspendable one and is
// laid out as its generated driver runs it rather than as a straight lookup.
// The key record must stay visible to the guest across the suspend boundary, so
// it lives in the guest stack frame; on the running exit the frame is handed
// back, and on suspension the frame, both parameters and the live cursor are
// pushed onto the asyncify stack instead, preceded by the resume point that
// says whether the general get still owes a call. The blocks stay apart in
// driver order: the two rewinding pops（frame first, then resume point — the
// reverse of the push order, which is the asyncify stack contract）, the running
// setup, the walk, the get, and one exit that either frees the frame and
// returns the slot or unwinds with a fresh suspend frame. Merging the two
// rewinding blocks, or folding the get's resume-point select into a
// short-circuit test, would change that order rather than merely read nicer.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

uint32_t GetTableFieldByString(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  // The scratch frame the key record is built in; on the rewind pass the frame
  // the save point pushed. The two roles are the generated body's own reuse of
  // one local, kept here so the rewind order below still reads back correctly.
  uint32_t frame = 0u;
  // The chain cursor, then the found or freshly looked-up value record itself.
  uint32_t node = 0u;
  // Which call site suspended this body. Zero is the general get below, the
  // only one there is.
  uint32_t resume_point = 0u;
  // Whether this pass issues the general get. Declared with the other locals
  // because the match path below leaves before the select that assigns it.
  uint32_t issue_get = 0u;
  // The key tag a node has to carry for the identity test to run, and the tag
  // stamped on the rebuilt key record.
  uint32_t const kTypeString = 4u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the save point pushed, then restore both
    // parameters plus the live cursor and the guest frame from it. The guest
    // frame is the last word read, so it replaces the frame pointer only after
    // every other word has been taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    node = LoadU32(ctx, frame + 8u);
    frame = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed together with that frame. Zero
    // means the general get never completed, so it is issued again below.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: carve the key record's frame and select the key's main position.
    // The bucket mask is the table's bucket shift with its low bits set, so an
    // empty table still selects a bucket instead of masking the hash away.
    frame = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = frame;
    uint32_t const hash_base = LoadU32(ctx, table + 16u);
    uint32_t const key_hash = LoadU32(ctx, key + 8u);
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const node_mask = (0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu;
    node = hash_base + ((key_hash & node_mask) << 5);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Walk the chain from that position: only nodes tagged as holding a string
    // key take part in the compare, and running off the chain means the key is
    // absent.
    bool matched = false;
    for (;;) {
      if (LoadU32(ctx, node + 24u) == kTypeString && LoadU32(ctx, node + 16u) == key) {
        matched = true;
        break;
      }
      node = LoadU32(ctx, node + 28u);
      if (node == 0u) {
        break;
      }
    }
    if (matched && node != kNilValueAddr) {
      // Found: the node's own value record is the answer, so leave with it,
      // past the general get below.
      goto return_path;
    }
    // Empty-handed: leave the key behind as a string value record for the
    // general get, which is where an __index metamethod would look.
    StoreU32(ctx, frame + 8u, kTypeString);
    StoreU32(ctx, frame, key);
  }
  // Running always owes the get; a rewind owes it only at the resume point it
  // popped above. The select reads the phase for truthiness, so any non-running
  // phase keeps the popped answer.
  issue_get = static_cast<uint32_t>(!resume_point);
  issue_get = ResumePhase(ctx) != kResumeRunning ? issue_get : 1u;
  if (issue_get) {
    key = FinishTableIndex(ctx, state, table, frame);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The general get ran guest code and suspended. The zero this call
      // reports is also what the suspend path saves as its resume point.
      key = 0u;
      goto suspend_point;
    }
    node = key;
  }
return_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = frame + 16u;
    LeaveFrame();
    return node;
  }
  TrapUnreachable();
suspend_point:;
  // Push the resume point, then the 16-byte frame holding both parameters, the
  // live cursor and the guest frame, and report the suspension as zero. The
  // frame the words are written through is a fresh address read back from the
  // stack pointer after it advanced, so it stays distinct from the scratch
  // frame the walk carved — that scratch frame is itself the fourth word saved
  // here, and the rewind prologue reads it back into the same local.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const suspend_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, suspend_frame, state);
  StoreU32(ctx, suspend_frame + 4u, table);
  StoreU32(ctx, suspend_frame + 8u, node);
  StoreU32(ctx, suspend_frame + 12u, frame);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
