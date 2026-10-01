#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/compare_bytes.cpp.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
// Forward declaration — defined in util/copy_bytes.cpp.
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);
// The three resumable callees below are defined elsewhere in the tree.
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t state);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
void ResizeStringTable(EngineContext* ctx, uint32_t state, uint32_t capacity);

namespace {

// The code a rewinding frame reads back names the call it was inside, so a
// resumed frame replays exactly that call and no other.
constexpr uint32_t kInternStringResumeBlockTooBig = 0u;
constexpr uint32_t kInternStringResumeAllocateNode = 1u;
constexpr uint32_t kInternStringResumeGrowTable = 2u;

// The tag a string node carries at +4, and the two low bits of its marked byte
// that pair the white generations.
constexpr uint32_t kInternStringTypeTag = 4u;
constexpr uint32_t kInternStringWhitePair = 3u;

// A range one byte short of this leaves no room for its terminator in the
// address space, so it is never built.
constexpr uint32_t kInternStringTerminatorLimit = 0xFFFFFFEEu;
// The largest table capacity the engine will still double.
constexpr uint32_t kInternStringMaxTableCapacity = 0x3FFFFFFEu;
// The node header plus the range's terminator byte.
constexpr uint32_t kInternStringNodeOverhead = 17u;

}  // namespace

uint32_t InternString(EngineContext* ctx, uint32_t state, uint32_t bytes, uint32_t length) {
  EnterFrame();
  // The four working slots below each carry several roles in turn: `node` is the
  // chain head and later the node the walk found, `scratch` the stride and later
  // a call's answer, `hold` the hash stride and later the table that owns the
  // chains, and `resume_point` names the call a rewinding frame has to replay.
  uint32_t node = 0u;
  uint32_t scratch = 0u;
  uint32_t hold = 0u;
  uint32_t hash = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: drop the frame pointer by the saved frame's size and read the
    // six words back out of it. The table that owns the chains is restored
    // last, into the very word the frame pointer was read through, so it
    // replaces that base only after every word has been taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 24u);
    hold = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, hold);
    bytes = LoadU32(ctx, hold + 4u);
    length = LoadU32(ctx, hold + 8u);
    node = LoadU32(ctx, hold + 12u);
    hash = LoadU32(ctx, hold + 16u);
    hold = LoadU32(ctx, hold + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the code naming the call that was in flight sits one word
    // above the saved frame, so it is popped after it. Zero names the
    // block-too-big raise below, the first of the three calls.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: mix the bytes into the hash, one per stride, seeded with the
    // length and walking backwards from the last byte. A range shorter than one
    // stride has no byte to fold in and keeps the seed as its hash.
    hash = length;
    hold = length >> 5u;
    if (hold < length) {
      scratch = hold ^ 0xFFFFFFFFu;
      node = length;
      do {
        hash ^= LoadU8(ctx, bytes + node - 1u) + ((hash << 5u) + (hash >> 2u));
        node += scratch;
      } while (hold < node);
    }
    // The hash's own slot in the table's pointer array, and the chain that slot
    // heads. An empty slot leaves nothing to walk, which is a miss like any
    // exhausted chain.
    hold = LoadU32(ctx, state + 16u);
    scratch = LoadU32(ctx, hold);
    node = LoadU32(ctx, scratch + ((hash & (LoadU32(ctx, hold + 8u) - 1u)) << 2u));
    scratch = static_cast<uint32_t>(node == 0u);
    if (scratch == 0u) {
      if (length != 0u) {
        for (;;) {
          // Length first, bytes only where the lengths agree: the engine never
          // compares a chain node that stores a different length.
          if (LoadU32(ctx, node + 12u) == length
              && CompareBytes(ctx, bytes, node + 16u, length) == 0u) {
            goto found_path;
          }
          node = LoadU32(ctx, node);
          if (node == 0u) {
            break;
          }
        }
      } else {
        for (;;) {
          // Nothing to compare, so a chain node storing a zero length is it.
          if (LoadU32(ctx, node + 12u) == 0u) {
            goto found_path;
          }
          node = LoadU32(ctx, node);
          if (node == 0u) {
            break;
          }
        }
      }
    }
    // The miss leaves the too-big flag behind: it arms the first gate below on
    // this running pass.
    node = static_cast<uint32_t>((length + 1u) >= kInternStringTerminatorLimit);
  }
  // The too-big raise. A running pass takes the gate only when the flag above
  // says the range cannot be built; a rewinding pass takes it to replay the
  // raise when the frame it popped names that call.
  if (node != 0u || ResumePhase(ctx) == kResumeRewinding) {
    uint32_t const issue_raise =
        ResumePhase(ctx) ? static_cast<uint32_t>(resume_point == kInternStringResumeBlockTooBig) : 1u;
    if (issue_raise) {
      scratch = RaiseBlockTooBig(ctx, state);
      if (ResumePhase(ctx) == kResumeSuspended) {
        // The raise ran guest code and suspended. It answers zero, so the block
        // value and the code word handed to the rewinder are the same word.
        scratch = kInternStringResumeBlockTooBig;
        goto suspend_point;
      }
      node = scratch;
    }
  }
  // The size to allocate. A running pass always has one; a rewinding pass only
  // has it once the raise above has been replayed, where the block value is
  // what the raise answered.
  node = ResumePhase(ctx) ? node : (length + kInternStringNodeOverhead);
  // The node's allocation. Running always owes it; a rewind owes it only at the
  // code it popped.
  {
    uint32_t const issue_alloc =
        ResumePhase(ctx) ? static_cast<uint32_t>(resume_point == kInternStringResumeAllocateNode) : 1u;
    if (issue_alloc) {
      scratch = ReallocateBlock(ctx, state, 0u, 0u, node);
      if (ResumePhase(ctx) == kResumeSuspended) {
        scratch = kInternStringResumeAllocateNode;
        goto suspend_point;
      }
      node = scratch;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: stamp the fresh node's header, copy the bytes in behind it and
    // terminate them, then thread it onto the head of its chain. Stamping the
    // marked byte is what keeps the node off the collector's list until the
    // generation it was built for comes round.
    StoreU32(ctx, node + 8u, hash);
    StoreU32(ctx, node + 12u, length);
    // The table pointer in `hold` gives way here to the generation byte the
    // table carries, which is the value that word holds from now on: the chains
    // are reached through the state from this point, and the suspend frame
    // saves the word as it stands.
    hold = LoadU8(ctx, LoadU32(ctx, state + 16u) + 20u);
    StoreU8(ctx, node + 6u, 0u);
    StoreU8(ctx, node + 4u, kInternStringTypeTag);
    StoreU8(ctx, node + 5u, hold & kInternStringWhitePair);
    scratch = CopyBytes(ctx, node + 16u, bytes, length);
    StoreU8(ctx, scratch + length, 0u);
    // The chain slot is computed a second time: the first walk only described
    // where the key would be, and this is the slot the node is linked into. The
    // length word is reused past this point, first as that slot's byte offset
    // and then as the table's entry count plus one.
    bytes = LoadU32(ctx, state + 16u);
    scratch = LoadU32(ctx, bytes);
    length = (hash & (LoadU32(ctx, bytes + 8u) - 1u)) << 2u;
    StoreU32(ctx, node, LoadU32(ctx, scratch + length));
    StoreU32(ctx, LoadU32(ctx, bytes) + length, node);
    length = LoadU32(ctx, bytes + 4u) + 1u;
    StoreU32(ctx, bytes + 4u, length);
    bytes = LoadU32(ctx, bytes + 8u);
    if (length <= bytes) {
      // The chain still fits: the table is left as it is.
      goto return_node;
    }
    if (static_cast<int32_t>(bytes) > static_cast<int32_t>(kInternStringMaxTableCapacity)) {
      // At the engine's ceiling the table is left as it is, however full.
      goto return_node;
    }
    bytes = bytes << 1u;
  }
  // The growth. Running owes it whenever the count above passed the capacity
  // and the capacity was still below the ceiling; a rewind owes it only at the
  // code it popped.
  {
    uint32_t const issue_grow =
        ResumePhase(ctx) ? static_cast<uint32_t>(resume_point == kInternStringResumeGrowTable) : 1u;
    if (issue_grow) {
      ResizeStringTable(ctx, state, bytes);
      if (ResumePhase(ctx) == kResumeSuspended) {
        scratch = kInternStringResumeGrowTable;
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass that got this far built a node and hands it back. The
    // found node's own path is the block below, reached only by the walk.
    goto return_node;
  }
found_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // A node the walk found is handed back as it stands, unless the generation
    // byte the table carries still leaves one of its marker bits unclaimed, in
    // which case that marker is flipped onto the other generation first.
    uint32_t const marked = LoadU8(ctx, node + 5u);
    uint32_t const generation = LoadU8(ctx, hold + 20u);
    if ((marked & (generation ^ 0xFFFFFFFFu) & kInternStringWhitePair) != 0u) {
      StoreU8(ctx, node + 5u, marked ^ kInternStringWhitePair);
    }
  }
return_node:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return node;
  }
  TrapUnreachable();
suspend_point:;
  // Hand the rewinder the code naming the call to replay, then the frame the
  // rewind prologue above reads back, and answer zero.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), scratch);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  scratch = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, scratch, state);
  StoreU32(ctx, scratch + 4u, bytes);
  StoreU32(ctx, scratch + 8u, length);
  StoreU32(ctx, scratch + 12u, node);
  StoreU32(ctx, scratch + 16u, hash);
  StoreU32(ctx, scratch + 20u, hold);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 24u);
  LeaveFrame();
  return 0u;
}
}  // namespace artc
