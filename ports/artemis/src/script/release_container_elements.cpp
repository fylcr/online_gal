// release_container_elements.cpp — dispose every element a script container
// owns.
//
// ReleaseContainerElements is the teardown walk behind a container's
// destructor. The container keeps four stores and the body visits them in that
// order: a bucket array of 8-byte head slots at +16 selected by the shift word
// at +56, a 12-byte-stride array counted by +64 whose every element keeps a
// 4-byte link pair at +32/+36, a second 12-byte-stride array counted by +68 and
// walked the same way, and two plain list heads at +72 and +80.
//
// An element is always relocated to the object that owns it first: the node's
// own first word names a descriptor whose -152 word is the delta back to the
// owner, and the owner's first word names a vtable whose +140 slot is the
// disposer. That slot is what every call here invokes, and the element itself
// is always its one argument. Two shapes of chain run through the nodes: one
// whose next link sits at +28, and one whose links come in 8-byte pairs where
// +8 holds the successor and the successor's own first word points back at the
// node that named it.
//
// Null pointers are the one thing this walk refuses. Each store's head is
// checked against the shared-pointer precondition the engine reports through
// its failure bridge — the boost assertion "px != 0" inside operator-> — and a
// head that fails it unwinds into the engine's catch path instead of being
// walked. The bridge takes a five-word vector that is fixed for every site in
// this body, so it is named once and reused at all four of them. Because the
// bridge and the disposer calls can both run guest code, this is a suspendable
// body and keeps the suspend driver's shape: a rewinding pass pops the
// 16-byte frame holding the container and the three cursors, then the word
// naming the call that was in flight, and every site is guarded on the phase
// so exactly that one call is replayed. The four bridge sites also re-read the
// engine's pending-throw slot immediately after the bridge returns, because a
// bridge that came back without raising is not a state this walk continues
// from: it falls into the unreachable guard, and only a raised flag reaches
// the catch path that hands the failure on to the abort product.
//
// Judgment order is the engine's own throughout. The head test is taken before
// the rewinding bit is merged into it; the relocation runs on a running pass
// while the disposer select runs on every pass; each store's head is tested
// against its own cursor rather than a shared one; and the two list heads at
// the tail keep their own pair of tests, including the one that asks whether
// the second head even differs from the first. Merging the two rewinding pops,
// or folding a bridge site's resume-code select into a short-circuit test,
// would change that order rather than merely read nicer.
#include "util/engine_context.h"

namespace artc {

// The failure handoff, the boost-precondition bridge, the catch matcher, the
// temp-return reader and the indirect dispatch are all named in
// util/engine_context.h, so this body calls them directly and declares none of
// them again.

namespace {

// The engine's pending-throw slot, cleared before every bridge call and read
// back after it. Registered as kPendingThrowAddr; the constant lives here
// until util/panic.cpp lands.
constexpr uint32_t kPendingThrowAddr = 0xD015Cu;
// The boost assertion the failure bridge reports out of
// shared_ptr::operator->: the unidentified leading word, the expression, its
// source file, the line and the enclosing function（see code/strings.tsv）.
constexpr uint32_t kAssertWord = 857u;
constexpr uint32_t kNullCheckTextAddr = 0x8D564u;
constexpr uint32_t kSharedPtrFileAddr = 0x8D4BEu;
constexpr uint32_t kArrowOperatorLine = 734u;
constexpr uint32_t kArrowOperatorTextAddr = 0x8D56Cu;
// The resume codes the driver spills at the eleven await sites. Zero is the
// first bridge site, which the driver tests with a bare zero rather than a
// compare; every later site numbers itself.
constexpr uint32_t kNullHeadAssert = 0u;
constexpr uint32_t kFirstDisposeCall = 1u;
constexpr uint32_t kNextHeadAssert = 2u;
constexpr uint32_t kSecondDisposeCall = 3u;
constexpr uint32_t kChainHeadAssert = 4u;
constexpr uint32_t kChainDisposeCall = 5u;
constexpr uint32_t kSecondChainAssert = 6u;
constexpr uint32_t kSecondChainDisposeCall = 7u;
constexpr uint32_t kAuxListDisposeCall = 8u;
constexpr uint32_t kLastListDisposeCall = 9u;
constexpr uint32_t kPanicHandoff = 10u;

}  // namespace

void ReleaseContainerElements(EngineContext* ctx, uint32_t container) {
  EnterFrame();
  // The walk's four reused slots: a node pointer, a slot that carries either a
  // null test or a read vtable entry, a store cursor, and the code naming the
  // call in flight, plus the temporary a phase gate is folded into. All five are
  // declared up here because the labels below are jumped over, jump past their
  // own assignment points, and because a rewinding pass restores every one of
  // them from the saved frame.
  uint32_t node = 0u;
  uint32_t slot = 0u;
  uint32_t cursor = 0u;
  uint32_t resume_code = 0u;
  uint32_t gate = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path pushed and rebuild the
    // container and the three cursors from it — the container first, then the
    // three slots in the order they were pushed. The frame word itself is read
    // last, so the cursor is only given up once every other word has been
    // taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    cursor = LoadU32(ctx, ReturnFrameSp(ctx));
    container = LoadU32(ctx, cursor);
    node = LoadU32(ctx, cursor + 4u);
    slot = LoadU32(ctx, cursor + 8u);
    cursor = LoadU32(ctx, cursor + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume code pushed together with that frame. Zero is
    // the first bridge site, which is the one site tested with a bare zero.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the first store's head is the 8-byte slot the shift word at +56
    // selects out of the array at +16. Whether that head is null is the latch
    // the bridge site below is gated on.
    node = LoadU32(ctx, container + 56u);
    node = LoadU32(ctx, ((node << 3) + container) + 16u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The bridge is about to run, so the pending-throw slot is cleared first.
      StoreU32(ctx, kPendingThrowAddr, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNullHeadAssert) : 1u) {
      HostFailAssertion(ctx, kAssertWord, kNullCheckTextAddr, kSharedPtrFileAddr,
                        kArrowOperatorLine, kArrowOperatorTextAddr);
      resume_code = kNullHeadAssert;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A bridge that returned without raising cannot be continued from: the
      // throw flag decides between the unreachable guard and the catch path.
      container = LoadU32(ctx, kPendingThrowAddr);
      StoreU32(ctx, kPendingThrowAddr, 0u);
      container = static_cast<uint32_t>(container != 1u);
      if (container) {
        goto no_throw_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: relocate the head to its owner and read the disposer out of the
    // owner's vtable at +140.
    node = node + LoadU32(ctx, LoadU32(ctx, node) - 152u);
    slot = LoadU32(ctx, LoadU32(ctx, node) + 140u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstDisposeCall) : 1u) {
    // The owner is the disposer's one argument and the vtable slot is its entry.
    CallTableFunction(ctx, slot, node);
    resume_code = kFirstDisposeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the second store's head, and the latch that says whether it even
    // differs from the shift word — the two agree exactly when the container
    // carries a single store to walk.
    node = LoadU32(ctx, container + 60u);
    slot = static_cast<uint32_t>(node != LoadU32(ctx, container + 56u));
  }
  gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: the head of that same store, selected by the cursor.
      node = LoadU32(ctx, (node << 3) + container + 16u);
      slot = static_cast<uint32_t>(node == 0u);
    }
    gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (gate) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowAddr, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNextHeadAssert) : 1u) {
        HostFailAssertion(ctx, kAssertWord, kNullCheckTextAddr, kSharedPtrFileAddr,
                          kArrowOperatorLine, kArrowOperatorTextAddr);
        resume_code = kNextHeadAssert;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        container = LoadU32(ctx, kPendingThrowAddr);
        StoreU32(ctx, kPendingThrowAddr, 0u);
        container = static_cast<uint32_t>(container != 1u);
        if (container) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: relocate that head and read its disposer.
      node = node + LoadU32(ctx, LoadU32(ctx, node) - 152u);
      slot = LoadU32(ctx, LoadU32(ctx, node) + 140u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondDisposeCall) : 1u) {
      CallTableFunction(ctx, slot, node);
      resume_code = kSecondDisposeCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the first element of the +64 store, whose link pair at +32/+36
    // brackets the elements this walk still owes.
    node = LoadU32(ctx, container + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // An empty first element means the whole store is already gone.
    if (node) {
      goto container_done;
    }
    slot = LoadU32(ctx, container + 64u);
    cursor = slot * 12u + container;
    node = LoadU32(ctx, cursor + 32u);
    cursor = static_cast<uint32_t>(node != cursor + 36u);
  }
  gate = cursor | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate) {
  bucket_walk:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: step the chain through the +28 next link; a null link is the
      // latch the bridge site below is gated on.
      cursor = node;
      node = LoadU32(ctx, node + 28u);
      slot = static_cast<uint32_t>(node == 0u);
    }
    gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (gate) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowAddr, 0u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kChainHeadAssert) : 1u) {
        HostFailAssertion(ctx, kAssertWord, kNullCheckTextAddr, kSharedPtrFileAddr,
                          kArrowOperatorLine, kArrowOperatorTextAddr);
        resume_code = kChainHeadAssert;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        container = LoadU32(ctx, kPendingThrowAddr);
        StoreU32(ctx, kPendingThrowAddr, 0u);
        container = static_cast<uint32_t>(container != 1u);
        if (container) {
          goto no_throw_path;
        }
        goto catch_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: the chain node's own disposer.
      slot = LoadU32(ctx, LoadU32(ctx, node) + 140u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kChainDisposeCall) : 1u) {
      CallTableFunction(ctx, slot, node);
      resume_code = kChainDisposeCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: which shape of chain this element carries. A second-word link
      // of zero sends the walk down the 8-byte pair shape from the element's
      // own +8 pair, and that walk stops when a successor's first word no
      // longer points back at the node that named it.
      slot = LoadU32(ctx, cursor + 4u);
      if (slot == 0u) {
        node = LoadU32(ctx, cursor + 8u);
        if (cursor == LoadU32(ctx, node)) {
          goto bucket_chain_done;
        }
        slot = cursor + 8u;
      chain_link_step:;
        cursor = LoadU32(ctx, slot);
        slot = cursor + 8u;
        node = LoadU32(ctx, cursor + 8u);
        if (LoadU32(ctx, node) != cursor) {
          goto chain_link_step;
        }
        goto bucket_chain_done;
      }
    next_link_walk:;
      // The other shape: a single next link at +0 that simply runs to null.
      node = slot;
      slot = LoadU32(ctx, slot);
      if (slot) {
        goto next_link_walk;
      }
    bucket_chain_done:;
      // One more element in the +64 store? The walk repeats while the chain end
      // has not reached the element past the last one. The count itself is read
      // back into `slot` first, because the next store's head is tested against
      // that same slot and the source leaves the count in it.
      slot = LoadU32(ctx, container + 64u);
      cursor = static_cast<uint32_t>(slot * 12u + container + 36u != node);
      if (cursor) {
        goto bucket_walk;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the +68 store's head, tested against the +64 store's count. The
    // two agree exactly when that store holds nothing to walk.
    node = LoadU32(ctx, container + 68u);
    slot = static_cast<uint32_t>(node == slot);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A store that starts where the previous one ended holds nothing new.
    if (slot) {
      goto second_walk_done;
    }
    slot = node * 12u + container;
    node = LoadU32(ctx, slot + 32u);
    slot = static_cast<uint32_t>(node == slot + 36u);
    if (slot) {
      goto second_walk_done;
    }
  }
bucket_walk_again:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: step the second store's chain through the +28 next link, and
    // latch on a null link for the bridge site below.
    cursor = node;
    node = LoadU32(ctx, node + 28u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowAddr, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondChainAssert) : 1u) {
      HostFailAssertion(ctx, kAssertWord, kNullCheckTextAddr, kSharedPtrFileAddr,
                        kArrowOperatorLine, kArrowOperatorTextAddr);
      resume_code = kSecondChainAssert;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      container = LoadU32(ctx, kPendingThrowAddr);
      StoreU32(ctx, kPendingThrowAddr, 0u);
      container = static_cast<uint32_t>(container != 1u);
      if (container) {
        goto no_throw_path;
      }
      goto catch_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the second store's chain node disposer.
    slot = LoadU32(ctx, LoadU32(ctx, node) + 140u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondChainDisposeCall) : 1u) {
    CallTableFunction(ctx, slot, node);
    resume_code = kSecondChainDisposeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the same two chain shapes, on the second store's elements. The
    // 8-byte pair shape keeps its own link step and its own done target.
    slot = LoadU32(ctx, cursor + 4u);
    if (slot == 0u) {
      node = LoadU32(ctx, cursor + 8u);
      if (cursor == LoadU32(ctx, node)) {
        goto second_chain_done;
      }
      slot = cursor + 8u;
    chain_link_step_b:;
      cursor = LoadU32(ctx, slot);
      slot = cursor + 8u;
      node = LoadU32(ctx, cursor + 8u);
      if (LoadU32(ctx, node) != cursor) {
        goto chain_link_step_b;
      }
      goto second_chain_done;
    }
  next_link_walk_b:;
    node = slot;
    slot = LoadU32(ctx, slot);
    if (slot) {
      goto next_link_walk_b;
    }
  second_chain_done:;
    // The second store ends where the +68 count says it does.
    slot = static_cast<uint32_t>(node != LoadU32(ctx, container + 68u) * 12u + container + 36u);
    if (slot) {
      goto bucket_walk_again;
    }
  }
second_walk_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the first list head at +72. An absent head skips both lists.
    node = LoadU32(ctx, container + 72u);
    if (node == 0u) {
      goto container_done;
    }
    node = node + LoadU32(ctx, LoadU32(ctx, node) - 152u);
    slot = LoadU32(ctx, LoadU32(ctx, node) + 140u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAuxListDisposeCall) : 1u) {
    CallTableFunction(ctx, slot, node);
    resume_code = kAuxListDisposeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the second list head at +80, which is only walked when it is
    // neither the same head as the first nor absent.
    node = LoadU32(ctx, container + 80u);
    container = static_cast<uint32_t>(node == LoadU32(ctx, container + 72u));
    if (container) {
      goto container_done;
    }
    container = static_cast<uint32_t>(node == 0u);
    if (container) {
      goto container_done;
    }
    container = node + LoadU32(ctx, LoadU32(ctx, node) - 152u);
    node = LoadU32(ctx, LoadU32(ctx, container) + 140u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kLastListDisposeCall) : 1u) {
    // The last call reverses the pair: the list node is the entry and the
    // relocated owner is the argument.
    CallTableFunction(ctx, node, container);
    resume_code = kLastListDisposeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
container_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
no_throw_path:;
  // Reached only when a bridge came back without raising, which the walk has no
  // state to continue from.
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
catch_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the exception is in flight. Ask the engine which catch it
    // matches — there is no type filter at this site, so the filter word is
    // zero — and let the bridge read the runner's return slot.
    container = FindMatchingCatch(ctx, 0u);
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPanicHandoff) : 1u) {
    Panic(ctx, container);
    resume_code = kPanicHandoff;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 16-byte frame a rewinding pass reads back — the container and the
    // three cursors. The stack pointer lives in the word the frame pointer
    // itself names, so every push is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, container);
    StoreU32(ctx, saved_frame + 4u, node);
    StoreU32(ctx, saved_frame + 8u, slot);
    StoreU32(ctx, saved_frame + 12u, cursor);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
