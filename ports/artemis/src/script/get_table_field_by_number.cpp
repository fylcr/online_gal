#include "util/engine_context.h"

namespace artc {

// Resolves table[key] for a key the caller knows is an integer: the flat array
// part first, else the 32-byte node chain hashed off the key's f64 bit pattern.
// A miss is not yet an answer -- the key is left behind as a number value record
// for the general, metamethod-capable get, which can run guest code. Answers the
// slot the lookup landed on, or the miss record. Suspendable.
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  // The chain cursor, then the found value record itself.
  uint32_t node = 0u;
  // The scratch frame the key record is built in; on the rewind pass, the frame
  // the suspend path pushed.
  uint32_t frame = 0u;
  // Which call site suspended this body; zero is the general get below, the only
  // one there is.
  uint32_t resume_point = 0u;
  // Whether the key-1 index lands inside the declared array part at all.
  uint32_t array_part_hit = 0u;
  // Whether this pass issues the general get.
  uint32_t issue_get = 0u;
  // Hoisted because the `goto`s below jump past where these would otherwise be
  // declared: the key's f64 bit pattern, the node array's base, that pattern mixed
  // down to a bucket index, and the table's bucket shift and derived divisor.
  uint64_t num_bits = 0u;
  double num = 0.0;
  uint32_t bucket_base = 0u;
  uint32_t mixed = 0u;
  uint32_t shift = 0u;
  uint32_t divisor = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved, and restore both
    // parameters plus the live cursor and the guest frame from it. The guest frame
    // is the last word read, so it replaces the frame pointer only after every
    // other word has been taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    node = LoadU32(ctx, frame + 8u);
    frame = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame. Zero means the
    // general get never completed, so it is issued again below.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: carve the key record's frame off the guest stack and decide up front
    // whether the key can land in the array part at all.
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
    // Hash part: mix the key's f64 bit pattern down to a bucket index. The divisor
    // is the bucket mask forced odd, so the remainder never divides by zero and key
    // zero hashes to the first bucket.
    bucket_base = LoadU32(ctx, table + 16u);
    num_bits = ReinterpretF64(num);
    mixed = static_cast<uint32_t>((num_bits >> 32) + num_bits);
    shift = LoadU8(ctx, table + 7u);
    divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
    node = bucket_base + ((mixed % divisor) << 5);
  hash_head:;
    // Walk the chain: only nodes tagged as numbers take part in the compare, and
    // the walk ends when the next link is null.
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
    // The chain walk and the array hit converge here: a chain entry that is itself
    // the miss record is the second way a lookup comes up empty.
    if (node != kNilValueAddr) {
      goto return_path;
    }
    num = static_cast<double>(static_cast<int32_t>(key));
  build_key_record:;
    // No field of that shape: leave the key behind as a number value record for the
    // general get, which is where an __index metamethod would look.
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
      // The general get ran guest code and suspended. The zero this call reports is
      // also what the suspend path saves as its resume point.
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
  // Push the resume point, then the 16-byte frame holding both parameters, the live
  // cursor and the guest frame, and report the suspension as zero.
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
