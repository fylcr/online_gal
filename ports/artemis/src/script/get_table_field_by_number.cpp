// get_table_field_by_number.cpp — integer-keyed table get with a suspendable
// general fallback.
//
// GetTableFieldByNumber resolves table[key] for a key the caller already knows
// is an integer. The resolve is the VM's usual two-shape lookup: a flat array
// part indexed by key-1 below the declared length, else a 32-byte node chain
// keyed by the f64 bit pattern mixed down to a bucket. A resolve that lands on
// the shared miss record is not yet an answer, because the field may still come
// from an __index metamethod. A miss therefore rebuilds the key as a number
// value record（value f64 at +0, type tag 3 at +8）and hands the get to the
// general, metamethod-capable product.
//
// That general get can run guest code, so this body is a suspendable one and is
// laid out as its generated driver runs it rather than as a straight lookup.
// The key record must stay visible to the guest across the suspend boundary, so
// it is built in a 16-byte frame carved off the guest stack（which grows down）.
// On the running exit the frame is handed back; on suspension the frame, both
// parameters and the live lookup cursor are pushed onto the asyncify stack
// instead, preceded by the resume point that says whether the general get still
// owes a call. The blocks stay apart in driver order: the two rewinding pops
// （frame first, then resume point — the reverse of the push order, which is the
// asyncify stack contract）, the running setup, the resolve, the get, and one
// exit that either frees the frame and returns the slot or unwinds with a fresh
// suspend frame. Merging the two rewinding blocks, or folding the get's
// resume-point select into a short-circuit test, would change that order rather
// than merely read nicer.
//
// The chain walk and the array hit converge on one candidate test, which is why
// the cursor is compared against the miss record after the walk instead of
// inside it: a chain entry that is itself the miss record is the second way a
// lookup comes up empty, and both shapes fall into the key-record build. The
// hash divisor is the bucket mask forced odd, so it is never zero even for an
// empty table.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  // The chain cursor, then the found value record itself.
  uint32_t node = 0u;
  // The scratch frame the key record is built in; on the rewind pass the frame
  // the suspend path pushed.
  uint32_t frame = 0u;
  // Which call site suspended this body. Zero is the general get below, the
  // only one there is.
  uint32_t resume_point = 0u;
  // Whether the key-1 index lands inside the array part at all.
  uint32_t array_part_hit = 0u;
  // Whether this pass issues the general get. Declared with the other locals
  // because the found path below leaves before the select that assigns it.
  uint32_t issue_get = 0u;
  // The key's f64 bit pattern, the node array's base, that pattern mixed down to
  // a bucket index, the table's bucket shift and the divisor it builds. All are
  // hoisted here because the `goto`s below jump past the point where they would
  // otherwise be declared.
  uint64_t num_bits = 0u;
  double num = 0.0;
  uint32_t bucket_base = 0u;
  uint32_t mixed = 0u;
  uint32_t shift = 0u;
  uint32_t divisor = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved, and restore both
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
    // Running: carve the key record's frame, and decide up front whether the
    // key can land in the array part at all.
    frame = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = frame;
    node = key - 1u;
    array_part_hit = node < LoadU32(ctx, table + 28u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (array_part_hit) {
      // Array part: one 16-byte record per consecutive index.
      node = LoadU32(ctx, table + 12u) + (node << 4);
      goto candidate_ready;
    }
    num = static_cast<double>(static_cast<int32_t>(key));
    if (key == 0u) {
      // Number zero sits at the head of the node array.
      node = LoadU32(ctx, table + 16u);
      goto hash_head;
    }
    // Hash part: mix the key's f64 bit pattern down to a bucket index. The
    // divisor is the bucket mask forced odd, so the remainder never divides by
    // zero and key zero hashes to the first bucket. The generated order is kept:
    // the node array first, then the bit pattern, then the bucket-count byte.
    bucket_base = LoadU32(ctx, table + 16u);
    num_bits = ReinterpretF64(num);
    mixed = static_cast<uint32_t>((num_bits >> 32) + num_bits);
    shift = LoadU8(ctx, table + 7u);
    divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
    node = bucket_base + ((mixed % divisor) << 5);
  hash_head:;
    // Walk the chain: only nodes tagged as numbers take part in the compare,
    // and the walk ends when the next link is null.
    for (;;) {
      if (LoadU32(ctx, node + 24u) == 3u && LoadF64(ctx, node + 16u) == num) {
        break;
      }
      node = LoadU32(ctx, node + 28u);
      if (node == 0u) {
        goto build_key_record;
      }
    }
  candidate_ready:;
    if (node != kNilValueAddr) {
      goto return_path;
    }
    num = static_cast<double>(static_cast<int32_t>(key));
  build_key_record:;
    // No field of that shape: leave the key behind as a number value record
    // for the general get, which is where an __index metamethod would look.
    StoreU32(ctx, frame + 8u, 3u);
    StoreF64(ctx, frame, num);
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
  // live cursor and the guest frame, and report the suspension as zero.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), key);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, state);
  StoreU32(ctx, saved_frame + 4u, table);
  StoreU32(ctx, saved_frame + 8u, node);
  StoreU32(ctx, saved_frame + 12u, frame);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
