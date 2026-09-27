// intern_string.cpp — intern a byte range as a string node in the state's table.
//
// InternString turns a byte range and its length into the node the engine keys
// its string table by: the node that is already there when the range has been
// seen before, a freshly built one when it has not. The lookup is the engine's
// own. The bytes are mixed into a 32-bit hash — one byte per stride, seeded
// with the length and walking backwards — and that hash is masked down to one
// slot of the table's pointer array; the chain hanging off the slot is then
// walked with the stored length compared first and the bytes only where the
// lengths already agree. A zero-length range has no bytes to compare at all, so
// its walk stops at the first node whose stored length is zero. Coming up empty
// is not the end: the slot's chain is about to receive the new node, so the
// body goes on to build it.
//
// Building stamps the whole node header — the value tag, the marked byte, the
// spare byte, the hash and the length — copies the bytes in behind it,
// terminates them, and threads the node onto the head of its chain. The chain
// is linked through the table's current pointer array before the table is asked
// to grow, so a node that arrives together with a growth is rehashed into the
// larger array by that same step, and the growth is skipped entirely once the
// capacity is the engine's ceiling. One earlier gate stops the build outright:
// a range whose terminator would not fit an address is handed to the
// block-too-big raise instead of being built at all.
//
// The raise, the allocation of the node itself and the growth are guest calls
// that can suspend the worker, so the body carries the engine's asyncify state
// machine and is laid out as the generated driver runs it rather than as a
// straight lookup. Phase rewinding pops the frame the suspend path saved — both
// parameters, the length, the live node, the hash and one working word, in that
// order — and then pops the code naming the call that was in flight, because
// each of the three calls keeps its own gate and a rewind replays exactly the
// one the frame names. Phase suspended pushes that code, then the frame, and
// answers zero. The two rewinding pops stay two blocks: the code word sits
// above the saved frame, so folding them would read the wrong words. The found
// node's exit is phase-guarded the same way, because only a running pass hands
// a node back and a resuming frame has nothing left to say.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/compare_bytes.cpp.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
// Forward declaration — defined in util/copy_bytes.cpp.
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);
// Forward declarations — the three resumable callees below are still generated
// bodies（f250 raises the block-too-big error、f223 is the block allocator and
// f1114 rehashes the string table into a new capacity), so both their product
// names and their product files are provisional.
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t state);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
void ResizeStringTable(EngineContext* ctx, uint32_t state, uint32_t capacity);

namespace {

// The code a rewinding frame reads back names the call it was inside, so a
// resumed frame replays exactly that call and no other.
constexpr uint32_t kResumeBlockTooBig = 0u;
constexpr uint32_t kResumeAllocateNode = 1u;
constexpr uint32_t kResumeGrowTable = 2u;

// The tag a string node carries at +4, and the two low bits of its marked byte
// that pair the white generations.
constexpr uint32_t kTypeString = 4u;
constexpr uint32_t kWhitePair = 3u;

// A range one byte short of this leaves no room for its terminator in the
// address space, so it is never built.
constexpr uint32_t kTerminatorLimit = 0xFFFFFFEEu;
// The largest table capacity the engine will still double.
constexpr uint32_t kMaxTableCapacity = 0x3FFFFFFEu;
// The node header plus the range's terminator byte.
constexpr uint32_t kNodeOverhead = 17u;

}  // namespace

uint32_t InternString(EngineContext* ctx, uint32_t state, uint32_t bytes, uint32_t length) {
  EnterFrame();
  // One name per generated slot, across every role that slot plays. `node` is
  // the bucket's chain head, then the node the walk found, then the too-big
  // flag a miss leaves behind, and finally the fresh node itself. `scratch` is
  // the backwards chain stride, then the pointer array's base, then the flag
  // that says the slot was empty, then the generation byte the table carries,
  // then a call's answer, then the chain slot's byte offset, and at the very
  // end the frame base the suspend push writes through. `hold` is the hash
  // stride and then the table that owns the chains. `hash` is the mixed hash
  // and `resume_point` names the call a rewinding frame has to replay.
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
    node = static_cast<uint32_t>((length + 1u) >= kTerminatorLimit);
  }
  // The too-big raise. A running pass takes the gate only when the flag above
  // says the range cannot be built; a rewinding pass takes it to replay the
  // raise when the frame it popped names that call.
  if (node != 0u || ResumePhase(ctx) == kResumeRewinding) {
    uint32_t const issue_raise =
        ResumePhase(ctx) ? static_cast<uint32_t>(resume_point == kResumeBlockTooBig) : 1u;
    if (issue_raise) {
      scratch = RaiseBlockTooBig(ctx, state);
      if (ResumePhase(ctx) == kResumeSuspended) {
        // The raise ran guest code and suspended. It answers zero, so the block
        // value and the code word handed to the rewinder are the same word.
        scratch = kResumeBlockTooBig;
        goto spill_frame;
      }
      node = scratch;
    }
  }
  // The size to allocate. A running pass always has one; a rewinding pass only
  // has it once the raise above has been replayed, where the block value is
  // what the raise answered.
  node = ResumePhase(ctx) ? node : (length + kNodeOverhead);
  // The node's allocation. Running always owes it; a rewind owes it only at the
  // code it popped.
  {
    uint32_t const issue_alloc =
        ResumePhase(ctx) ? static_cast<uint32_t>(resume_point == kResumeAllocateNode) : 1u;
    if (issue_alloc) {
      scratch = ReallocateBlock(ctx, state, 0u, 0u, node);
      if (ResumePhase(ctx) == kResumeSuspended) {
        scratch = kResumeAllocateNode;
        goto spill_frame;
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
    StoreU8(ctx, node + 4u, kTypeString);
    StoreU8(ctx, node + 5u, hold & kWhitePair);
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
    if (static_cast<int32_t>(bytes) > static_cast<int32_t>(kMaxTableCapacity)) {
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
        ResumePhase(ctx) ? static_cast<uint32_t>(resume_point == kResumeGrowTable) : 1u;
    if (issue_grow) {
      ResizeStringTable(ctx, state, bytes);
      if (ResumePhase(ctx) == kResumeSuspended) {
        scratch = kResumeGrowTable;
        goto spill_frame;
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
    // A node the walk found, handed back as it stands unless the generation
    // byte the table carries still leaves one of its marker bits unclaimed, in
    // which case that marker is flipped onto the other generation first. The
    // generated body parks the marker byte in its state parameter, which is
    // dead from here on, so it is kept in a word of its own.
    uint32_t const marked = LoadU8(ctx, node + 5u);
    uint32_t const generation = LoadU8(ctx, hold + 20u);
    if ((marked & (generation ^ 0xFFFFFFFFu) & kWhitePair) != 0u) {
      StoreU8(ctx, node + 5u, marked ^ kWhitePair);
    }
  }
return_node:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return node;
  }
  TrapUnreachable();
spill_frame:;
  // Hand the rewinder the code naming the call to replay, then the frame this
  // body's own rewind prologue reads back — both parameters, the length, the
  // live node, the hash and the working word — and answer zero. The frame the
  // words are written through is read back from the stack pointer after it
  // advanced, so it is a fresh address rather than the frame base the rewind
  // prologue reads.
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
